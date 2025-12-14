const DATE_REGEX = /^\d{4}-\d{2}-\d{2}$/;
const DATETIME_REGEX = /^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$/;
let cachedProjects = [];
let cachedTasks = [];
let cachedReminders = [];
let cachedAchievements = [];
let cachedHeatmapData = [];
let pomoTimer = null;
let pomoEndTime = null;
let pomoMode = null;
let pomoPaused = false;
let pomoRemainingMs = 0;
let pomoTotalMs = 0;
let reminderCheckInterval = null;
let remindersEnabled = true;
let pendingReminderQueue = [];
let previousXpLevel = 0;
let previousAchievements = [];

async function fetchJSON(url) {
  const r = await fetch(url);
  if (!r.ok) throw new Error(r.statusText);
  return r.json();
}

async function post(url) {
  const r = await fetch(url, { method: "POST" });
  if (!r.ok) throw new Error(r.statusText);
  return r.json();
}

function cleanEmptyFields(data) {
  const cleaned = { ...data };
  Object.keys(cleaned).forEach((k) => {
    if (cleaned[k] === "") delete cleaned[k];
  });
  return cleaned;
}

function pad(v) {
  return String(v).padStart(2, "0");
}

function toDateTimeString(inputValue) {
  if (!inputValue) return "";
  const dt = new Date(inputValue);
  if (Number.isNaN(dt.getTime())) return "";
  return `${dt.getFullYear()}-${pad(dt.getMonth() + 1)}-${pad(dt.getDate())} ${pad(dt.getHours())}:${pad(dt.getMinutes())}:${pad(dt.getSeconds())}`;
}

function isValidDateStrict(dateStr) {
  const m = dateStr.match(/^(\d{4})-(\d{2})-(\d{2})$/);
  if (!m) return false;
  const [_, y, mo, d] = m;
  const dt = new Date(`${y}-${mo}-${d}T00:00:00Z`);
  return (
    dt.getUTCFullYear() === Number(y) &&
    dt.getUTCMonth() + 1 === Number(mo) &&
    dt.getUTCDate() === Number(d)
  );
}

function isValidDateTimeStrict(dateStr) {
  const m = dateStr.match(/^(\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2})$/);
  if (!m) return false;
  const [_, y, mo, d, h, mi, s] = m;
  if (Number(h) > 23 || Number(mi) > 59 || Number(s) > 59) return false;
  return isValidDateStrict(`${y}-${mo}-${d}`);
}

function renderList(container, items, formatter) {
  container.innerHTML = "";
  items.forEach((it) => {
    const div = document.createElement("div");
    div.className = "item";
    div.innerHTML = formatter(it);
    container.appendChild(div);
  });
}

// ============================================
// REMINDER NOTIFICATION SYSTEM (Apple-style)
// ============================================
function showReminderPopup(reminder) {
  if (!remindersEnabled) return;
  
  // Remove any existing popup
  const existing = document.querySelector('.reminder-overlay');
  if (existing) existing.remove();
  
  const overlay = document.createElement('div');
  overlay.className = 'reminder-overlay';
  overlay.innerHTML = `
    <div class="reminder-popup">
      <div class="reminder-popup-header">
        <span class="reminder-popup-icon">🔔</span>
        <div>
          <h3 class="reminder-popup-title">${escapeHtml(reminder.title)}</h3>
          <div class="reminder-popup-time">⏰ ${reminder.time}</div>
        </div>
      </div>
      <div class="reminder-popup-message">
        ${escapeHtml(reminder.message || 'Time for your reminder!')}
      </div>
      <div class="reminder-popup-actions">
        <button class="reminder-btn-dismiss" data-action="dismiss-reminder" data-id="${reminder.id}">
          ✓ Dismiss
        </button>
        <button class="reminder-btn-snooze" data-action="snooze-reminder" data-id="${reminder.id}">
          ⏰ Snooze 5min
        </button>
      </div>
    </div>
  `;
  
  document.body.appendChild(overlay);
  
  // Play notification sound
  playNotificationSound();
  
  // Also try browser notification
  if ("Notification" in window && Notification.permission === "granted") {
    new Notification(reminder.title, { 
      body: reminder.message || 'Time for your reminder!',
      icon: '🔔'
    });
  }
}

function dismissReminderPopup() {
  const overlay = document.querySelector('.reminder-overlay');
  if (overlay) {
    overlay.style.animation = 'fadeIn 0.3s ease reverse';
    setTimeout(() => overlay.remove(), 300);
  }
}

async function snoozeReminder(reminderId, minutes = 5) {
  try {
    const now = new Date();
    now.setMinutes(now.getMinutes() + minutes);
    const newTime = `${now.getFullYear()}-${pad(now.getMonth() + 1)}-${pad(now.getDate())} ${pad(now.getHours())}:${pad(now.getMinutes())}:${pad(now.getSeconds())}`;
    await post(`/api/reminders/reschedule?id=${reminderId}&time=${encodeURIComponent(newTime)}`);
    dismissReminderPopup();
  } catch (err) {
    console.error('Failed to snooze reminder:', err);
  }
}

function playNotificationSound() {
  // Simple beep using Web Audio API
  try {
    // Check if AudioContext is available
    const AudioContextClass = window.AudioContext || window.webkitAudioContext;
    if (!AudioContextClass) return;
    
    const audioContext = new AudioContextClass();
    const oscillator = audioContext.createOscillator();
    const gainNode = audioContext.createGain();
    
    oscillator.connect(gainNode);
    gainNode.connect(audioContext.destination);
    
    oscillator.frequency.value = 800;
    oscillator.type = 'sine';
    gainNode.gain.setValueAtTime(0.3, audioContext.currentTime);
    gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.5);
    
    oscillator.start(audioContext.currentTime);
    oscillator.stop(audioContext.currentTime + 0.5);
  } catch (e) {
    // Audio not supported or failed
  }
}

async function checkDueReminders() {
  if (!remindersEnabled) return;
  
  try {
    const pending = await fetchJSON('/api/reminders/pending');
    const now = new Date();
    
    for (const reminder of pending) {
      const reminderTime = new Date(reminder.time.replace(' ', 'T'));
      // Check if reminder is due (within last minute)
      if (reminderTime <= now && (now - reminderTime) < 60000) {
        // Check if we haven't already shown this reminder
        if (!pendingReminderQueue.includes(reminder.id)) {
          pendingReminderQueue.push(reminder.id);
          showReminderPopup(reminder);
        }
      }
    }
  } catch (err) {
    console.error('Failed to check reminders:', err);
  }
}

function startReminderChecker() {
  // Check every 10 seconds instead of continuously
  if (reminderCheckInterval) clearInterval(reminderCheckInterval);
  reminderCheckInterval = setInterval(checkDueReminders, 10000);
}

function stopReminderChecker() {
  if (reminderCheckInterval) {
    clearInterval(reminderCheckInterval);
    reminderCheckInterval = null;
  }
}

function toggleReminders(enabled) {
  remindersEnabled = enabled;
  if (enabled) {
    startReminderChecker();
  } else {
    stopReminderChecker();
    dismissReminderPopup();
  }
}

function escapeHtml(text) {
  const div = document.createElement('div');
  div.textContent = text;
  return div.innerHTML;
}

// ============================================
// INTERACTIVE HEATMAP (GitHub-style)
// ============================================
function generateHeatmapHTML(heatmapData) {
  const months = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'];
  const days = ['', 'Mon', '', 'Wed', '', 'Fri', ''];
  
  // Validate input
  if (heatmapData === null || heatmapData === undefined) {
    heatmapData = '';
  }
  
  // Parse heatmap data from text format or use API data
  const taskCounts = parseHeatmapData(heatmapData);
  
  // Generate 90 days of cells
  const cells = [];
  const today = new Date();
  
  for (let i = 89; i >= 0; i--) {
    const date = new Date(today);
    date.setDate(date.getDate() - i);
    const dateStr = `${date.getFullYear()}-${pad(date.getMonth() + 1)}-${pad(date.getDate())}`;
    const count = taskCounts[dateStr] || 0;
    const level = count === 0 ? 0 : count <= 2 ? 1 : count <= 4 ? 2 : count <= 6 ? 3 : 4;
    
    cells.push({
      date: dateStr,
      count,
      level,
      dayOfWeek: date.getDay()
    });
  }
  
  // Build HTML
  let html = `
    <div class="heatmap-container">
      <div class="heatmap-header">
        <div class="heatmap-title">📊 Activity Heatmap (90 days)</div>
      </div>
      <div class="heatmap-months">
        ${getMonthLabels(cells)}
      </div>
      <div class="heatmap-grid-wrapper">
        <div class="heatmap-days">
          ${days.map(d => `<div class="heatmap-day-label">${d}</div>`).join('')}
        </div>
        <div class="heatmap-grid">
  `;
  
  // Arrange cells by week columns
  cells.forEach(cell => {
    html += `
      <div class="heatmap-cell" data-level="${cell.level}" data-date="${cell.date}" title="${cell.date}: ${cell.count} tasks">
        <div class="heatmap-tooltip">${cell.date}<br>${cell.count} tasks completed</div>
      </div>
    `;
  });
  
  html += `
        </div>
      </div>
      <div class="heatmap-legend">
        <span>Less</span>
        <div class="heatmap-legend-cell" style="background:#161b22"></div>
        <div class="heatmap-legend-cell" style="background:#0e4429"></div>
        <div class="heatmap-legend-cell" style="background:#006d32"></div>
        <div class="heatmap-legend-cell" style="background:#26a641"></div>
        <div class="heatmap-legend-cell" style="background:#39d353"></div>
        <span>More</span>
      </div>
    </div>
  `;
  
  return html;
}

function parseHeatmapData(data) {
  const counts = {};
  
  // First try to use the data array from the API (preferred method)
  if (data && typeof data === 'object' && Array.isArray(data.data)) {
    data.data.forEach(item => {
      if (item.date && item.count !== undefined) {
        counts[item.date] = item.count;
      }
    });
    return counts;
  }
  
  // If data is an array directly
  if (Array.isArray(data)) {
    data.forEach(item => {
      if (item.date && item.count !== undefined) {
        counts[item.date] = item.count;
      }
    });
    return counts;
  }
  
  // Fallback: parse text-based heatmap output
  if (typeof data === 'string') {
    // Look for date patterns
    const dateRegex = /(\d{4}-\d{2}-\d{2})/g;
    let match;
    while ((match = dateRegex.exec(data)) !== null) {
      counts[match[1]] = (counts[match[1]] || 0) + 1;
    }
  }
  
  return counts;
}

function getMonthLabels(cells) {
  const months = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'];
  let currentMonth = -1;
  let html = '';
  
  cells.forEach((cell, index) => {
    const date = new Date(cell.date);
    if (date.getMonth() !== currentMonth) {
      currentMonth = date.getMonth();
      if (index % 7 === 0) {
        html += `<span class="heatmap-month">${months[currentMonth]}</span>`;
      }
    }
  });
  
  return html;
}

// ============================================
// GAMIFICATION (Duolingo-style)
// ============================================
function showXPCelebration(xpGained, message = 'Great job!') {
  const celebration = document.createElement('div');
  celebration.className = 'xp-celebration';
  celebration.innerHTML = `
    <div class="xp-celebration-icon">⭐</div>
    <div class="xp-celebration-text">+${xpGained} XP</div>
    <div class="xp-celebration-sub">${message}</div>
  `;
  
  document.body.appendChild(celebration);
  spawnConfetti();
  
  setTimeout(() => {
    celebration.style.animation = 'celebrationPop 0.3s ease reverse';
    setTimeout(() => celebration.remove(), 300);
  }, 2000);
}

function showAchievementUnlock(achievement) {
  const toast = document.createElement('div');
  toast.className = 'achievement-unlock-toast';
  toast.innerHTML = `
    <div class="achievement-unlock-icon">🏆</div>
    <div class="achievement-unlock-text">
      <div class="achievement-unlock-title">Achievement Unlocked!</div>
      <div class="achievement-unlock-name">${achievement.name}</div>
    </div>
  `;
  
  document.body.appendChild(toast);
  spawnConfetti();
  
  setTimeout(() => {
    toast.style.animation = 'slideInRight 0.5s ease reverse';
    setTimeout(() => toast.remove(), 500);
  }, 4000);
}

function showLevelUp(newLevel, title) {
  const celebration = document.createElement('div');
  celebration.className = 'xp-celebration';
  celebration.style.borderColor = '#8b5cf6';
  celebration.innerHTML = `
    <div class="xp-celebration-icon">🎉</div>
    <div class="xp-celebration-text">Level ${newLevel}!</div>
    <div class="xp-celebration-sub">${title}</div>
  `;
  
  document.body.appendChild(celebration);
  spawnConfetti();
  
  setTimeout(() => {
    celebration.style.animation = 'celebrationPop 0.3s ease reverse';
    setTimeout(() => celebration.remove(), 300);
  }, 3000);
}

function spawnConfetti() {
  const colors = ['#fbbf24', '#10b981', '#8b5cf6', '#ef4444', '#06b6d4'];
  
  for (let i = 0; i < 30; i++) {
    const confetti = document.createElement('div');
    confetti.className = 'confetti';
    confetti.style.left = Math.random() * 100 + 'vw';
    confetti.style.top = '100vh';
    confetti.style.background = colors[Math.floor(Math.random() * colors.length)];
    confetti.style.animationDelay = Math.random() * 0.5 + 's';
    confetti.style.animationDuration = 1.5 + Math.random() * 1 + 's';
    
    document.body.appendChild(confetti);
    
    setTimeout(() => confetti.remove(), 3000);
  }
}

// Group projects by color for visual distinction
function groupProjectsByColor(projects) {
  const groups = {};
  projects.forEach(p => {
    const color = p.color || "#4CAF50";
    if (!groups[color]) groups[color] = [];
    groups[color].push(p);
  });
  return groups;
}

// Get color name from hex
function getColorName(hex) {
  const colorMap = {
    '#F44336': 'Red',
    '#E91E63': 'Pink', 
    '#9C27B0': 'Purple',
    '#3F51B5': 'Indigo',
    '#2196F3': 'Blue',
    '#00BCD4': 'Cyan',
    '#4CAF50': 'Green',
    '#FFC107': 'Amber',
    '#FF5722': 'Deep Orange',
    '#795548': 'Brown',
    '#607D8B': 'Blue Grey'
  };
  return colorMap[hex] || hex;
}

// Calculate days until due
function getDaysUntilDue(dueDate) {
  if (!dueDate) return null;
  const due = new Date(dueDate);
  const today = new Date();
  today.setHours(0, 0, 0, 0);
  due.setHours(0, 0, 0, 0);
  return Math.ceil((due - today) / (1000 * 60 * 60 * 24));
}

// Get urgency class based on due date
function getUrgencyClass(dueDate) {
  const days = getDaysUntilDue(dueDate);
  if (days === null) return '';
  if (days < 0) return 'urgent';
  if (days <= 2) return 'warning';
  return 'safe';
}

async function load() {
  try {
    const [tasks, projects, reminders] = await Promise.all([
      fetchJSON("/api/tasks"),
      fetchJSON("/api/projects"),
      fetchJSON("/api/reminders"),
    ]);
    cachedProjects = projects;
    cachedTasks = tasks;
    cachedReminders = reminders;

    // update selects
    const projSelect = document.getElementById("task-project-select");
    projSelect.innerHTML = `<option value="">(No project)</option>` +
      projects.map(p => `<option value="${p.id}">${p.name}</option>`).join("");

    const reminderTaskSelect = document.getElementById("reminder-task-select");
    reminderTaskSelect.innerHTML = `<option value="">(No task)</option>` +
      tasks.map(t => `<option value="${t.id}">${t.name}</option>`).join("");

    // Enhanced task display
    document.getElementById("task-count").textContent = `${tasks.length} items`;
    const taskListEl = document.getElementById("task-list");
    taskListEl.innerHTML = tasks.map(t => {
      const priorityClass = t.priority === 2 ? 'high-priority' : t.priority === 1 ? 'medium-priority' : 'low-priority';
      const priorityText = t.priority === 2 ? '🔴 High' : t.priority === 1 ? '🟡 Medium' : '🟢 Low';
      const daysUntil = getDaysUntilDue(t.due);
      const urgencyClass = getUrgencyClass(t.due);
      const dueText = daysUntil !== null ? (daysUntil < 0 ? `${Math.abs(daysUntil)} days overdue` : daysUntil === 0 ? 'Due today!' : `${daysUntil} days left`) : 'No deadline';
      
      return `
        <div class="task-item ${priorityClass} ${t.completed ? 'completed' : ''}">
          <div class="task-header">
            <div class="task-title-section">
              <div class="task-name">${t.completed ? '✅ ' : ''}${escapeHtml(t.name)}</div>
              ${t.desc ? `<div class="task-desc">${escapeHtml(t.desc)}</div>` : ''}
            </div>
          </div>
          <div class="task-meta-grid">
            <div class="task-meta-item">
              <span class="task-meta-label">Priority</span>
              <span class="task-meta-value">${priorityText}</span>
            </div>
            <div class="task-meta-item">
              <span class="task-meta-label">Deadline</span>
              <span class="task-meta-value ${urgencyClass}">${t.due || 'Not set'}</span>
            </div>
            <div class="task-meta-item">
              <span class="task-meta-label">Time Left</span>
              <span class="task-meta-value ${urgencyClass}">${dueText}</span>
            </div>
            <div class="task-meta-item">
              <span class="task-meta-label">Estimate</span>
              <span class="task-meta-value">🍅 ${t.estimated || 0} pomodoro</span>
            </div>
          </div>
          <div class="task-badges">
            ${t.projectName ? `<span class="task-badge project" style="border-color:${t.projectColor || '#4CAF50'}">📁 ${escapeHtml(t.projectName)}</span>` : ''}
            ${t.tags ? `<span class="task-badge">🏷️ ${escapeHtml(t.tags)}</span>` : ''}
            ${daysUntil !== null && daysUntil < 0 ? '<span class="task-badge overdue">⚠️ Overdue</span>' : ''}
          </div>
          <div class="controls" style="margin-top: 12px;">
            <button class="btn-gamified ${t.completed ? '' : 'btn-complete'}" data-action="complete" data-id="${t.id}">${t.completed ? '↩️ Undo' : '✅ Complete'}</button>
            <button data-action="edit-task" data-id="${t.id}">✏️ Edit</button>
            <button data-action="assign" data-id="${t.id}">📎 Assign</button>
            <button data-action="delete" data-id="${t.id}">🗑️ Delete</button>
          </div>
        </div>
      `;
    }).join('');

    document.getElementById("project-count").textContent = `${projects.length} projects`;
    
    // Enhanced project display grouped by color
    const projectList = document.getElementById("project-list");
    const colorGroups = groupProjectsByColor(projects);
    let projectHtml = '';
    
    Object.entries(colorGroups).forEach(([color, projs]) => {
      const colorName = getColorName(color);
      projectHtml += `
        <div class="project-section" style="--section-color: ${color}">
          <div class="project-section-header" style="--section-color: ${color}">
            <div class="project-section-icon" style="background: ${color}">${colorName.charAt(0)}</div>
            <span class="project-section-title">${colorName} Projects</span>
            <span class="project-section-count">${projs.length} project${projs.length > 1 ? 's' : ''}</span>
          </div>
      `;
      
      projs.forEach(p => {
        const completedTasks = (p.tasks || []).filter(t => t.completed).length;
        const totalTasks = (p.tasks || []).length;
        const progressPercent = totalTasks > 0 ? (completedTasks / totalTasks * 100).toFixed(0) : 0;
        
        projectHtml += `
          <div class="project-card" style="--project-color: ${p.color || '#4CAF50'}">
            <div class="project-card-header">
              <div class="project-card-title">
                <div class="project-color-indicator" style="background: ${p.color || '#4CAF50'}"></div>
                ${escapeHtml(p.name)}
              </div>
              <div class="controls">
                <button data-action="proj-update" data-id="${p.id}">✏️</button>
                <button data-action="proj-delete" data-id="${p.id}">🗑️</button>
              </div>
            </div>
            ${p.description ? `<div class="muted micro" style="margin-bottom: 12px">${escapeHtml(p.description)}</div>` : ''}
            
            <div class="project-progress-section">
              <div style="display: flex; justify-content: space-between; align-items: center">
                <span class="muted micro">Progress: ${completedTasks}/${totalTasks} tasks</span>
                <span style="font-weight: 700; color: ${p.color || '#4CAF50'}">${progressPercent}%</span>
              </div>
              <div class="project-progress-bar">
                <div class="project-progress-fill" style="width: ${progressPercent}%; background: ${p.color || '#4CAF50'}"></div>
              </div>
              ${p.target ? `<div class="muted micro" style="margin-top: 8px">📅 Target: ${p.target}</div>` : ''}
            </div>
            
            <div class="project-tasks-section">
              <div class="project-tasks-header">
                <span style="font-weight: 600">Tasks</span>
                <span class="muted micro">${totalTasks} total</span>
              </div>
              <div class="project-task-list">
                ${(p.tasks || []).length > 0 ? (p.tasks || []).map(t => `
                  <div class="project-task-row">
                    <div class="project-task-checkbox ${t.completed ? 'completed' : ''}">${t.completed ? '✓' : ''}</div>
                    <span class="project-task-name ${t.completed ? 'completed' : ''}">${escapeHtml(t.name)}</span>
                    ${t.due ? `<span class="project-task-due">${t.due}</span>` : ''}
                  </div>
                `).join('') : '<div class="muted micro" style="padding: 10px">No tasks assigned yet</div>'}
              </div>
            </div>
          </div>
        `;
      });
      
      projectHtml += '</div>';
    });
    
    projectList.innerHTML = projectHtml || '<div class="muted">No projects yet. Create one below!</div>';

    // Enhanced reminders with toggle
    document.getElementById("reminder-count").textContent = `${reminders.length} reminders`;
    const reminderListEl = document.getElementById("reminder-list");
    
    // Add reminder toggle at top
    let reminderHtml = `
      <div class="reminder-settings">
        <label class="reminder-toggle">
          <input type="checkbox" id="reminder-toggle-input" ${remindersEnabled ? 'checked' : ''}>
          <span class="reminder-toggle-slider"></span>
        </label>
        <span>Notifications ${remindersEnabled ? 'enabled' : 'disabled'}</span>
        <span class="muted micro">(Toggle off if experiencing lag)</span>
      </div>
    `;
    
    reminders.forEach(r => {
      reminderHtml += `
        <div class="item">
          <div>
            <div class="title">🔔 ${escapeHtml(r.title)}</div>
            <div class="meta">⏰ ${r.time} · 🔄 ${r.recurrence} ${r.enabled ? '' : '⚠️ (disabled)'}</div>
            <div class="meta small">${escapeHtml(r.message || 'No message')} ${r.taskId ? ' · 📎 Task #' + r.taskId : ''}</div>
          </div>
          <div class="controls">
            <button data-action="rem-delete" data-id="${r.id}">🗑️ Delete</button>
            <button data-action="rem-reschedule" data-id="${r.id}">📅 Reschedule</button>
            <button data-action="rem-edit" data-id="${r.id}">✏️ Edit</button>
            <span class="chip">#${r.id}</span>
          </div>
        </div>
      `;
    });
    
    reminderListEl.innerHTML = reminderHtml;
    
    // Add toggle listener
    document.getElementById('reminder-toggle-input')?.addEventListener('change', (e) => {
      toggleReminders(e.target.checked);
    });

    await loadXPAndAchievements();
    await updatePomoState();
    await loadStatsSummary();
    await loadHeatmap();
  } catch (err) {
    console.error(err);
    alert("Failed to load data: " + err.message);
  }
}

async function loadHeatmap() {
  try {
    const res = await fetchJSON('/api/stats/heatmap');
    // Use the full response (which contains 'data' array with date-count pairs)
    cachedHeatmapData = res;
    
    // Insert heatmap into heatmap wrapper
    const heatmapWrapper = document.getElementById('heatmap-wrapper');
    if (heatmapWrapper) {
      heatmapWrapper.innerHTML = generateHeatmapHTML(cachedHeatmapData);
    }
  } catch (err) {
    console.error('Failed to load heatmap:', err);
  }
}

async function loadStatsSummary() {
  try {
    const stats = await fetchJSON("/api/stats/summary");
    const tasksEl = document.getElementById("stat-tasks-completed");
    const rateEl = document.getElementById("stat-completion-rate");
    const streakEl = document.getElementById("stat-streak");
    const pomosEl = document.getElementById("stat-pomodoros");
    
    if (tasksEl) tasksEl.textContent = stats.tasksCompleted || 0;
    if (rateEl) rateEl.textContent = `${((stats.completionRate || 0) * 100).toFixed(0)}%`;
    if (streakEl) {
      streakEl.textContent = stats.streak || 0;
      // Add streak fire animation if streak > 0
      const streakCard = streakEl.closest('.stat-card');
      if (streakCard && stats.streak > 0) {
        streakCard.querySelector('.stat-icon').innerHTML = `<span class="streak-fire"><span class="streak-fire-icon">🔥</span></span>`;
      }
    }
    if (pomosEl) pomosEl.textContent = stats.pomodoros || 0;
  } catch (e) {
    console.error("Failed to load stats summary:", e);
  }
}

function displayFormattedStats(kind, res) {
  const statBox = document.getElementById("stat-box");
  if (!statBox) return;

  // Parse the report string from the backend
  const reportText = res.report || res.heatmap || '';
  
  if (kind === 'summary') {
    // Format summary stats
    const summaryHtml = `
      <div class="stats-detail-grid">
        <div class="stats-detail-card">
          <div class="stats-detail-icon">✅</div>
          <div class="stats-detail-content">
            <div class="stats-detail-value">${res.tasksCompleted || 0}</div>
            <div class="stats-detail-label">Tasks Completed</div>
          </div>
        </div>
        <div class="stats-detail-card">
          <div class="stats-detail-icon">📊</div>
          <div class="stats-detail-content">
            <div class="stats-detail-value">${((res.completionRate || 0) * 100).toFixed(1)}%</div>
            <div class="stats-detail-label">Completion Rate</div>
          </div>
        </div>
        <div class="stats-detail-card">
          <div class="stats-detail-icon">🔥</div>
          <div class="stats-detail-content">
            <div class="stats-detail-value">${res.streak || 0}</div>
            <div class="stats-detail-label">Current Streak</div>
          </div>
        </div>
        <div class="stats-detail-card">
          <div class="stats-detail-icon">🏆</div>
          <div class="stats-detail-content">
            <div class="stats-detail-value">${res.longestStreak || 0}</div>
            <div class="stats-detail-label">Longest Streak</div>
          </div>
        </div>
        <div class="stats-detail-card">
          <div class="stats-detail-icon">🍅</div>
          <div class="stats-detail-content">
            <div class="stats-detail-value">${res.pomodoros || 0}</div>
            <div class="stats-detail-label">Total Pomodoros</div>
          </div>
        </div>
        <div class="stats-detail-card">
          <div class="stats-detail-icon">⏰</div>
          <div class="stats-detail-content">
            <div class="stats-detail-value">${res.pomodorosToday || 0}</div>
            <div class="stats-detail-label">Pomodoros Today</div>
          </div>
        </div>
      </div>
    `;
    statBox.innerHTML = summaryHtml;
  } else if (kind === 'daily' || kind === 'weekly' || kind === 'monthly') {
    // Parse the report text and format it nicely
    const formattedReport = formatReportText(reportText, kind);
    statBox.innerHTML = formattedReport;
  } else {
    statBox.textContent = reportText || JSON.stringify(res, null, 2);
  }
}

function formatReportText(reportText, kind) {
  if (!reportText) {
    return `<div class="stats-empty">No ${kind} data available</div>`;
  }
  
  // Parse the text-based report into sections
  // Handle both escaped newlines (from JSON) and actual newlines
  const lines = reportText.split(/\\n|\n/).filter(line => line.trim());
  
  const titleMap = {
    daily: '📅 Daily Report',
    weekly: '📆 Weekly Report',
    monthly: '📊 Monthly Report'
  };
  
  let html = `
    <div class="stats-report">
      <div class="stats-report-header">
        <h3>${titleMap[kind] || 'Report'}</h3>
      </div>
      <div class="stats-report-content">
  `;
  
  lines.forEach(line => {
    const trimmedLine = line.trim();
    if (!trimmedLine) return;
    
    // Check for section headers (lines with === or ---)
    if (trimmedLine.includes('===') || trimmedLine.includes('---')) {
      html += `<div class="stats-report-divider"></div>`;
    } 
    // Check for key-value pairs (contains : or =)
    else if (trimmedLine.includes(':')) {
      const [key, ...valueParts] = trimmedLine.split(':');
      const value = valueParts.join(':').trim();
      html += `
        <div class="stats-report-item">
          <span class="stats-report-key">${escapeHtml(key.trim())}</span>
          <span class="stats-report-value">${escapeHtml(value)}</span>
        </div>
      `;
    }
    // Regular text lines
    else {
      html += `<div class="stats-report-text">${escapeHtml(trimmedLine)}</div>`;
    }
  });
  
  html += `
      </div>
    </div>
  `;
  
  return html;
}

async function loadXPAndAchievements() {
  try {
    const [xp, ach] = await Promise.all([
      fetchJSON("/api/xp"),
      fetchJSON("/api/achievements"),
    ]);
    
    // Check for level up (only if we have previous data)
    if (previousXpLevel > 0 && xp.level > previousXpLevel) {
      showLevelUp(xp.level, xp.title);
    }
    previousXpLevel = xp.level;
    
    // Check for new achievements (only show unlock animation if we have previous data)
    // On first load, previousAchievements is empty, so we don't show animations
    if (previousAchievements.length > 0) {
      const newUnlocked = ach.filter(a => 
        a.unlocked && !previousAchievements.find(p => p.id === a.id && p.unlocked)
      );
      newUnlocked.forEach(a => showAchievementUnlock(a));
    }
    previousAchievements = ach.map(a => ({ id: a.id, unlocked: a.unlocked }));
    
    cachedAchievements = ach;
    
    const percent = xp.next > 0 ? Math.min(100, (xp.xp / xp.next) * 100) : 100;
    const bar = document.getElementById("xp-bar");
    if (bar) bar.style.width = `${percent}%`;
    const inlineBar = document.getElementById("xp-bar-inline");
    if (inlineBar) inlineBar.style.width = `${percent}%`;

    const levelText = `Level ${xp.level} · ${xp.title}`;
    const progressText = `${xp.xp}/${xp.next} XP`;
    const pill = document.getElementById("xp-level-pill");
    if (pill) {
      pill.innerHTML = `<span class="level-up-badge">⚡ Level ${xp.level}</span>`;
    }
    const levelEl = document.getElementById("xp-level-text");
    if (levelEl) levelEl.textContent = levelText;
    const progEl = document.getElementById("xp-progress-text");
    if (progEl) progEl.textContent = `${progressText} to next`;
    const titleEl = document.getElementById("xp-title-text");
    if (titleEl) titleEl.textContent = `Title: ${xp.title}`;
    const inlineText = document.getElementById("xp-inline-text");
    if (inlineText) inlineText.textContent = levelText + " · " + progressText;

    const achList = document.getElementById("achievements-list");
    const achCount = document.getElementById("ach-count");
    if (achCount) {
      const unlockedCount = ach.filter(a => a.unlocked).length;
      achCount.textContent = `${unlockedCount}/${ach.length} unlocked`;
    }
    if (achList) {
      achList.innerHTML = ach
        .map(
          (a) => `
            <div class="ach-item ${a.unlocked ? 'unlocked' : ''}" data-ach-id="${a.id}" style="${a.unlocked ? 'border-color: #fbbf24; background: rgba(251, 191, 36, 0.1)' : ''}">
              <div class="title">${a.unlocked ? '🏆' : '🔒'} ${a.name || ("Achievement #"+a.id)}</div>
              <div class="percent" style="${a.unlocked ? 'color: #fbbf24' : ''}">${(a.percent || 0).toFixed(1)}%</div>
              <div class="xp-track mini" style="margin: 8px 0">
                <div class="xp-fill" style="width: ${a.percent || 0}%; background: ${a.unlocked ? 'linear-gradient(135deg, #fbbf24, #f59e0b)' : 'linear-gradient(135deg, #7c3aed, #06b6d4)'}"></div>
              </div>
              <div class="muted micro">${a.progress}/${a.target} progress · ${a.unlocked ? "✅ Unlocked!" : "⏳ In progress"}</div>
              <div class="muted micro">${a.description || "Complete to unlock!"}</div>
            </div>
          `
        )
        .join("");
    }
    
    // Update achievement select dropdown
    const achSelect = document.getElementById("achievement-select");
    if (achSelect) {
      achSelect.innerHTML = `<option value="">Select an achievement to edit...</option>` +
        ach.map(a => `<option value="${a.id}">${a.name || ("Achievement #"+a.id)} (${a.progress}/${a.target})</option>`).join("");
    }
    
    const achInline = document.getElementById("achievements-inline");
    if (achInline) {
      achInline.innerHTML = ach
        .slice(0, 3)
        .map(
          (a) =>
            `<span class="ach-chip" style="${a.unlocked ? 'background: rgba(251, 191, 36, 0.2); border-color: #fbbf24' : ''}">${a.unlocked ? '🏆' : '🔒'} ${a.name || ("#"+a.id)} · ${(a.percent || 0).toFixed(0)}%</span>`
        )
        .join("");
    }
  } catch (e) {
    console.error("XP/Achievements load failed:", e);
  }
}

// Achievement select change handler - populate fields
document.getElementById("achievement-select")?.addEventListener("change", (e) => {
  const id = parseInt(e.target.value, 10);
  const ach = cachedAchievements.find(a => a.id === id);
  if (ach) {
    document.getElementById("ach-name-input").value = ach.name || "";
    document.getElementById("ach-desc-input").value = ach.description || "";
    document.getElementById("ach-target-input").value = ach.target || "";
  } else {
    document.getElementById("ach-name-input").value = "";
    document.getElementById("ach-desc-input").value = "";
    document.getElementById("ach-target-input").value = "";
  }
});

// Forms
document.getElementById("task-form").addEventListener("submit", async (e) => {
  e.preventDefault();
  const data = cleanEmptyFields(Object.fromEntries(new FormData(e.target).entries()));
  if (data.due && !isValidDateStrict(data.due)) {
    alert("Invalid due date. Use YYYY-MM-DD");
    return;
  }
  const qs = new URLSearchParams(data).toString();
  await post("/api/tasks/create?" + qs);
  await load();
  e.target.reset();
});

document.getElementById("project-form").addEventListener("submit", async (e) => {
  e.preventDefault();
  const data = cleanEmptyFields(Object.fromEntries(new FormData(e.target).entries()));
  if (data.target && !isValidDateStrict(data.target)) {
    alert("Invalid target date. Use YYYY-MM-DD");
    return;
  }
  const qs = new URLSearchParams(data).toString();
  await post("/api/projects/create?" + qs);
  await load();
  e.target.reset();
});

document.getElementById("reminder-form").addEventListener("submit", async (e) => {
  e.preventDefault();
  const data = cleanEmptyFields(Object.fromEntries(new FormData(e.target).entries()));
  data.time = toDateTimeString(data.time);
  if (!data.time) {
    alert("Please select a date and time for the reminder.");
    return;
  }
  if (!isValidDateTimeStrict(data.time)) {
    alert("Invalid time format. Please use the date picker to select a valid date and time.");
    return;
  }
  const qs = new URLSearchParams(data).toString();
  try {
    await post("/api/reminders/create?" + qs);
    await load();
    e.target.reset();
  } catch (err) {
    alert("Failed to create reminder: " + err.message);
  }
});

document.getElementById("achievement-form").addEventListener("submit", async (e) => {
  e.preventDefault();
  const data = cleanEmptyFields(Object.fromEntries(new FormData(e.target).entries()));
  if (!data.id) {
    alert("Please select an achievement to edit");
    return;
  }
  const qs = new URLSearchParams(data).toString();
  try {
    await post("/api/achievements/update?" + qs);
    await loadXPAndAchievements();
    e.target.reset();
    alert("Achievement updated successfully!");
  } catch (err) {
    alert("Failed to update achievement: " + err.message);
  }
});

// Delegated buttons
document.body.addEventListener("click", async (e) => {
  const btn = e.target;
  if (!(btn instanceof HTMLButtonElement)) return;
  const action = btn.dataset.action;
  const id = btn.dataset.id;
  try {
    if (action === "complete") {
      const task = cachedTasks.find(t => String(t.id) === String(id));
      const wasCompleted = task?.completed;
      await post(`/api/tasks/complete?id=${id}`);
      
      // Show celebration if task was just completed
      if (!wasCompleted) {
        const xpGained = task?.priority === 2 ? 30 : task?.priority === 1 ? 20 : 10;
        showXPCelebration(xpGained, 'Task completed!');
      }
    } else if (action === "delete") {
      if (!confirm("Are you sure you want to delete this task?")) return;
      await post(`/api/tasks/delete?id=${id}`);
    } else if (action === "assign") {
      const choice = prompt("Project ID? Available: " + cachedProjects.map(p=>p.id+":"+p.name).join(", "));
      if (!choice) return;
      const pid = parseInt(choice, 10);
      if (Number.isNaN(pid)) { alert("Invalid project id"); return; }
      await post(`/api/tasks/assign?id=${id}&projectId=${pid}`);
    } else if (action === "edit-task") {
      const task = cachedTasks.find(t => String(t.id) === String(id));
      const name = prompt("Task name", task?.name || "") ?? task?.name ?? "";
      const desc = prompt("Description", task?.desc || "") ?? task?.desc ?? "";
      const due = prompt("Due date YYYY-MM-DD (blank to keep)", task?.due || "") || "";
      if (due && !isValidDateStrict(due)) { alert("Invalid date"); return; }
      const priorityInput = prompt("Priority (0 low /1 medium /2 high)", task?.priority ?? 1);
      const priority = priorityInput === null ? task?.priority : parseInt(priorityInput, 10);
      if (Number.isNaN(priority)) { alert("Priority must be a number 0-2"); return; }
      const tags = prompt("Tags (comma separated)", task?.tags || "") ?? task?.tags ?? "";
      const est = prompt("Estimated pomodoro count", task?.estimated ?? 0);
      const qs = new URLSearchParams(cleanEmptyFields({
        id,
        name,
        desc,
        due,
        priority,
        tags,
        estPomodoro: est || ""
      })).toString();
      await post(`/api/tasks/update?${qs}`);
    } else if (action === "proj-delete") {
      if (!confirm("Are you sure you want to delete this project?")) return;
      await post(`/api/projects/delete?id=${id}`);
    } else if (action === "proj-update") {
      const current = cachedProjects.find(p => String(p.id) === String(id));
      const name = prompt("Name?", current?.name || "");
      const desc = prompt("Description?", current?.description || "");
      const color = prompt("Color (hex, e.g. #4CAF50)", current?.color || "#4CAF50") || current?.color || "#4CAF50";
      const target = prompt("Target date YYYY-MM-DD (blank keep)", current?.target || "") || "";
      if (target && !isValidDateStrict(target)) { alert("Invalid target date"); return; }
      const qs = new URLSearchParams(cleanEmptyFields({ id, name, desc, color, target })).toString();
      await post(`/api/projects/update?${qs}`);
    } else if (action === "rem-delete") {
      if (!confirm("Are you sure you want to delete this reminder?")) return;
      await post(`/api/reminders/delete?id=${id}`);
    } else if (action === "rem-reschedule") {
      const current = cachedReminders.find(r => String(r.id) === String(id));
      const t = prompt("New time YYYY-MM-DD HH:MM:SS", current?.time || "");
      if (!t) return;
      if (!isValidDateTimeStrict(t)) { alert("Invalid time format. Use YYYY-MM-DD HH:MM:SS"); return; }
      await post(`/api/reminders/reschedule?id=${id}&time=${encodeURIComponent(t)}`);
    } else if (action === "rem-edit") {
      const current = cachedReminders.find(r => String(r.id) === String(id));
      const title = prompt("Title", current?.title || "") ?? current?.title ?? "";
      const message = prompt("Message", current?.message || "") ?? current?.message ?? "";
      const timeInput = prompt("Time YYYY-MM-DD HH:MM:SS (blank keep)", current?.time || "") || "";
      if (timeInput && !isValidDateTimeStrict(timeInput)) { alert("Invalid time. Use YYYY-MM-DD HH:MM:SS"); return; }
      const recurrence = prompt("Recurrence (once/daily/weekly/monthly)", current?.recurrence || "once") || current?.recurrence || "once";
      const taskId = prompt("Link task id (blank for none)", current?.taskId || "") || "";
      const enabled = confirm("Should this reminder stay enabled?");
      const qs = new URLSearchParams(cleanEmptyFields({
        id,
        title,
        message,
        time: timeInput,
        recurrence,
        taskId,
        enabled
      })).toString();
      await post(`/api/reminders/update?${qs}`);
    } else if (action === "dismiss-reminder") {
      // Dismiss reminder popup and mark as triggered on backend
      await post(`/api/reminders/dismiss?id=${id}`);
      dismissReminderPopup();
      pendingReminderQueue = pendingReminderQueue.filter(rid => rid !== parseInt(id, 10));
    } else if (action === "snooze-reminder") {
      await snoozeReminder(parseInt(id, 10), 5);
      pendingReminderQueue = pendingReminderQueue.filter(rid => rid !== parseInt(id, 10));
    } else if (action === "pomo-work") {
      await post("/api/pomodoro/start");
      startPomoTimer(25, "Work Session");
    } else if (action === "pomo-break") {
      await post("/api/pomodoro/break");
      startPomoTimer(5, "Short Break");
    } else if (action === "pomo-long") {
      await post("/api/pomodoro/longbreak");
      startPomoTimer(15, "Long Break");
    } else if (action === "pomo-pause") {
      pausePomoTimer();
    } else if (action === "pomo-resume") {
      resumePomoTimer();
    } else if (action === "pomo-abandon") {
      if (confirm("Are you sure you want to abandon this Pomodoro session? This session will not be counted.")) {
        // First stop the backend pomodoro session
        try {
          await post("/api/pomodoro/stop");
        } catch (e) {
          console.warn("Failed to stop backend pomodoro:", e);
        }
        abandonPomoTimer();
      }
    } else if (btn.dataset.stat) {
      const kind = btn.dataset.stat;
      if (kind === 'heatmap') {
        // Load visual heatmap
        await loadHeatmap();
        document.getElementById("stat-box").innerHTML = '<div class="stat-message">Visual heatmap loaded above. Hover over cells to see details.</div>';
      } else {
        const res = await fetchJSON(`/api/stats/${kind}`);
        displayFormattedStats(kind, res);
      }
    }
    await load();
  } catch (err) {
    alert(err);
  }
});

// Pomodoro Timer Functions
function startPomoTimer(minutes, mode) {
  stopPomoTimer();
  pomoMode = mode;
  pomoPaused = false;
  pomoTotalMs = minutes * 60 * 1000;
  pomoEndTime = Date.now() + pomoTotalMs;
  
  const timerEl = document.getElementById("pomo-timer");
  const modeEl = document.getElementById("pomo-mode");
  const progressBar = document.getElementById("pomo-progress-bar");
  const statusEl = document.getElementById("pomo-status");
  
  if (modeEl) modeEl.textContent = mode;
  if (statusEl) statusEl.textContent = `${mode} started - ${minutes} minutes`;
  
  updatePomoPauseButtons(true);
  
  pomoTimer = setInterval(async () => {
    const remaining = Math.max(0, pomoEndTime - Date.now());
    const mins = Math.floor(remaining / 60000);
    const secs = Math.floor((remaining % 60000) / 1000);
    
    if (timerEl) timerEl.textContent = `${pad(mins)}:${pad(secs)}`;
    
    const progress = ((pomoTotalMs - remaining) / pomoTotalMs) * 100;
    if (progressBar) progressBar.style.width = `${progress}%`;
    
    if (remaining <= 0) {
      stopPomoTimer();
      if (timerEl) timerEl.textContent = "00:00";
      if (modeEl) modeEl.textContent = "✅ " + mode + " Complete!";
      if (statusEl) statusEl.textContent = mode + " completed!";
      
      // Award XP for completed work session
      if (mode === "Work Session") {
        try {
          await post("/api/pomodoro/complete");
          showXPCelebration(5, 'Pomodoro completed!');
        } catch (e) {
          console.warn("Failed to record pomodoro completion:", e);
        }
      }
      
      // Play notification sound or show alert
      try {
        if ("Notification" in window && Notification.permission === "granted") {
          new Notification("Pomodoro", { body: mode + " completed!" });
        } else {
          alert("⏰ " + mode + " completed!");
        }
      } catch (e) {
        // Fallback to alert if notifications fail
        alert("⏰ " + mode + " completed!");
      }
    }
  }, 1000);
}

function pausePomoTimer() {
  if (!pomoTimer || pomoPaused) return;
  
  clearInterval(pomoTimer);
  pomoTimer = null;
  pomoPaused = true;
  pomoRemainingMs = Math.max(0, pomoEndTime - Date.now());
  
  const modeEl = document.getElementById("pomo-mode");
  const statusEl = document.getElementById("pomo-status");
  
  if (modeEl) modeEl.textContent = "⏸️ " + pomoMode + " (Paused)";
  if (statusEl) statusEl.textContent = `${pomoMode} paused`;
  
  updatePomoPauseButtons(false);
}

function resumePomoTimer() {
  if (!pomoPaused || pomoRemainingMs <= 0) return;
  
  pomoPaused = false;
  pomoEndTime = Date.now() + pomoRemainingMs;
  
  const timerEl = document.getElementById("pomo-timer");
  const modeEl = document.getElementById("pomo-mode");
  const progressBar = document.getElementById("pomo-progress-bar");
  const statusEl = document.getElementById("pomo-status");
  
  if (modeEl) modeEl.textContent = pomoMode;
  if (statusEl) statusEl.textContent = `${pomoMode} resumed`;
  
  updatePomoPauseButtons(true);
  
  pomoTimer = setInterval(async () => {
    const remaining = Math.max(0, pomoEndTime - Date.now());
    const mins = Math.floor(remaining / 60000);
    const secs = Math.floor((remaining % 60000) / 1000);
    
    if (timerEl) timerEl.textContent = `${pad(mins)}:${pad(secs)}`;
    
    const progress = ((pomoTotalMs - remaining) / pomoTotalMs) * 100;
    if (progressBar) progressBar.style.width = `${progress}%`;
    
    if (remaining <= 0) {
      stopPomoTimer();
      if (timerEl) timerEl.textContent = "00:00";
      if (modeEl) modeEl.textContent = "✅ " + pomoMode + " Complete!";
      if (statusEl) statusEl.textContent = pomoMode + " completed!";
      
      // Award XP for completed work session
      if (pomoMode === "Work Session") {
        try {
          await post("/api/pomodoro/complete");
          showXPCelebration(5, 'Pomodoro completed!');
        } catch (e) {
          console.warn("Failed to record pomodoro completion:", e);
        }
      }
      
      try {
        if ("Notification" in window && Notification.permission === "granted") {
          new Notification("Pomodoro", { body: pomoMode + " completed!" });
        } else {
          alert("⏰ " + pomoMode + " completed!");
        }
      } catch (e) {
        alert("⏰ " + pomoMode + " completed!");
      }
    }
  }, 1000);
}

function updatePomoPauseButtons(showPause) {
  const pauseBtn = document.getElementById("pomo-pause-btn");
  const resumeBtn = document.getElementById("pomo-resume-btn");
  
  if (pauseBtn) pauseBtn.style.display = showPause ? "inline-block" : "none";
  if (resumeBtn) resumeBtn.style.display = showPause ? "none" : "inline-block";
}

function stopPomoTimer() {
  if (pomoTimer) {
    clearInterval(pomoTimer);
    pomoTimer = null;
  }
  pomoPaused = false;
  pomoRemainingMs = 0;
  
  const timerEl = document.getElementById("pomo-timer");
  const modeEl = document.getElementById("pomo-mode");
  const progressBar = document.getElementById("pomo-progress-bar");
  const statusEl = document.getElementById("pomo-status");
  
  if (timerEl) timerEl.textContent = "--:--";
  if (modeEl) modeEl.textContent = "Ready to start";
  if (progressBar) progressBar.style.width = "0%";
  if (statusEl) statusEl.textContent = "Pomodoro stopped.";
  
  updatePomoPauseButtons(false);
  // Hide both buttons when stopped
  const pauseBtn = document.getElementById("pomo-pause-btn");
  const resumeBtn = document.getElementById("pomo-resume-btn");
  if (pauseBtn) pauseBtn.style.display = "none";
  if (resumeBtn) resumeBtn.style.display = "none";
}

function abandonPomoTimer() {
  // Abandon the current Pomodoro session without recording it
  if (pomoTimer) {
    clearInterval(pomoTimer);
    pomoTimer = null;
  }
  pomoPaused = false;
  pomoRemainingMs = 0;
  
  const timerEl = document.getElementById("pomo-timer");
  const modeEl = document.getElementById("pomo-mode");
  const progressBar = document.getElementById("pomo-progress-bar");
  const statusEl = document.getElementById("pomo-status");
  
  if (timerEl) timerEl.textContent = "--:--";
  if (modeEl) modeEl.textContent = "Session Abandoned 🚫";
  if (progressBar) progressBar.style.width = "0%";
  if (statusEl) statusEl.textContent = "Pomodoro session abandoned. This session was discarded and will not be counted.";
  
  updatePomoPauseButtons(false);
  // Hide pause/resume buttons when abandoned
  const pauseBtn = document.getElementById("pomo-pause-btn");
  const resumeBtn = document.getElementById("pomo-resume-btn");
  if (pauseBtn) pauseBtn.style.display = "none";
  if (resumeBtn) resumeBtn.style.display = "none";
}

function setupNavigation() {
  const links = Array.from(document.querySelectorAll(".topnav a[data-target]"));
  const sections = Array.from(document.querySelectorAll("section[data-view]"));
  const show = (view) => {
    sections.forEach((sec) => {
      const match = sec.dataset.view === view;
      sec.classList.toggle("hidden", !match);
    });
    links.forEach((l) => l.classList.toggle("active", l.dataset.target === view));
  };
  links.forEach((link) => {
    link.addEventListener("click", () => show(link.dataset.target));
  });
  show("work");
}

async function updatePomoState() {
  try {
    const state = await fetchJSON("/api/pomodoro/state");
    const statusEl = document.getElementById("pomo-status");
    const totalCyclesEl = document.getElementById("pomo-total-cycles");
    
    if (statusEl && !pomoTimer) {
      statusEl.textContent = `Running: ${state.running ? "Yes" : "No"} · Cycles: ${state.cycles}`;
    }
    if (totalCyclesEl) {
      totalCyclesEl.textContent = state.cycles || 0;
    }
  } catch {
    const statusEl = document.getElementById("pomo-status");
    if (statusEl) statusEl.textContent = "Pomodoro state unavailable";
  }
}

// ============================================
// WELCOME ANIMATION
// ============================================
function hideWelcomeOverlay() {
  const overlay = document.getElementById('welcome-overlay');
  if (overlay) {
    overlay.classList.add('fade-out');
    setTimeout(() => {
      overlay.style.display = 'none';
    }, 800);
  }
}

// Auto-hide welcome overlay after loading completes
function initWelcomeAnimation() {
  // Wait for loading bar animation (2s) plus a small buffer
  setTimeout(hideWelcomeOverlay, 2200);
}

// ============================================
// EXIT ANIMATION & FUNCTIONALITY
// ============================================
function showExitOverlay() {
  const exitOverlay = document.getElementById('exit-overlay');
  if (exitOverlay) {
    exitOverlay.style.display = 'flex';
    
    // Auto close the page after showing exit animation
    setTimeout(() => {
      // Try to close the window (works if opened by script)
      // Otherwise, provide instructions
      try {
        window.close();
      } catch (e) {
        // If window.close() doesn't work, show a message
        const exitContent = exitOverlay.querySelector('.exit-content');
        if (exitContent) {
          const message = document.createElement('p');
          message.style.marginTop = '20px';
          message.style.color = '#fbbf24';
          message.style.fontSize = '14px';
          message.innerHTML = 'You can safely close this tab now.<br>Press <kbd>Ctrl+W</kbd> (or <kbd>Cmd+W</kbd> on Mac) to close.';
          exitContent.appendChild(message);
        }
      }
    }, 2500);
  }
}

function setupExitButton() {
  const exitBtn = document.getElementById('exit-btn');
  if (exitBtn) {
    exitBtn.addEventListener('click', (e) => {
      e.preventDefault();
      if (confirm('Are you sure you want to exit? Your progress has been automatically saved.')) {
        showExitOverlay();
      }
    });
  }
  
  // Also listen for Ctrl+Q (keyboard shortcut for exit)
  document.addEventListener('keydown', (e) => {
    // Ctrl+Q or Cmd+Q for exit
    if ((e.ctrlKey || e.metaKey) && e.key === 'q') {
      e.preventDefault();
      if (confirm('Are you sure you want to exit?')) {
        showExitOverlay();
      }
    }
  });
}

// Request notification permission on load
if ("Notification" in window && Notification.permission === "default") {
  Notification.requestPermission();
}

setupNavigation();
setupExitButton();
initWelcomeAnimation();
load();
startReminderChecker();
