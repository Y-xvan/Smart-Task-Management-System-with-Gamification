const DATE_REGEX = /^\d{4}-\d{2}-\d{2}$/;
const DATETIME_REGEX = /^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$/;
let cachedProjects = [];
let cachedTasks = [];
let cachedReminders = [];
let cachedAchievements = [];
let pomoTimer = null;
let pomoEndTime = null;
let pomoMode = null;

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

    document.getElementById("task-count").textContent = `${tasks.length} items`;
    renderList(
      document.getElementById("task-list"),
      tasks,
      (t) => `
        <div style="border-left: 3px solid ${t.projectColor || '#1f2937'}; padding-left: 10px;">
          <div class="title">${t.name}</div>
          <div class="meta">${t.projectName ? `📁 <span style="color:${t.projectColor || '#4CAF50'}">${t.projectName}</span>` : "No project"} · Priority ${t.priority === 2 ? '🔴 High' : t.priority === 1 ? '🟡 Medium' : '🟢 Low'} · ${t.completed ? "✅ done" : "⏳ todo"}</div>
          <div class="meta small">Tags: ${t.tags || "none"} · Estimate: ${t.estimated || 0} pomodoro${t.due ? ` · Due: ${t.due}` : ''}</div>
        </div>
        <div class="controls">
          <button data-action="complete" data-id="${t.id}">${t.completed ? '↩️ Undo' : '✅ Complete'}</button>
          <button data-action="delete" data-id="${t.id}">🗑️ Delete</button>
          <button data-action="assign" data-id="${t.id}">📎 Assign</button>
          <button data-action="edit-task" data-id="${t.id}">✏️ Edit</button>
        </div>
      `
    );

    document.getElementById("project-count").textContent = `${projects.length} projects`;
    
    // Render projects grouped by color
    const projectList = document.getElementById("project-list");
    const colorGroups = groupProjectsByColor(projects);
    let projectHtml = '';
    
    Object.entries(colorGroups).forEach(([color, projs]) => {
      projectHtml += `<div class="project-group">
        <div class="project-group-header">
          <div class="project-color-dot" style="background:${color}"></div>
          <span class="project-group-title">${projs.length} project(s)</span>
        </div>`;
      
      projs.forEach(p => {
        const tasksList = (p.tasks || []).map(t => 
          `<div class="project-task-item ${t.completed ? 'completed' : ''}">
            ${t.completed ? '✅' : '⬜'} ${t.name}${t.due ? ` <span class="muted">(${t.due})</span>` : ''}
          </div>`
        ).join('') || '<div class="muted micro">No tasks assigned</div>';
        
        projectHtml += `
          <div class="item">
            <div class="project-head" style="border-left-color:${p.color || "#4CAF50"}">
              <div class="title">${p.name}</div>
              <div class="meta">${p.description || "No description"}</div>
              <div class="meta small">Progress: ${(p.progress * 100).toFixed(0)}% · Target: ${p.target || "Not set"}</div>
              <div class="project-tasks">${tasksList}</div>
            </div>
            <div class="controls">
              <button data-action="proj-delete" data-id="${p.id}">🗑️ Delete</button>
              <button data-action="proj-update" data-id="${p.id}">✏️ Edit</button>
              <span class="chip" style="background:${p.color || "#4CAF50"};color:#fff">${p.color || "#4CAF50"}</span>
            </div>
          </div>`;
      });
      
      projectHtml += '</div>';
    });
    
    projectList.innerHTML = projectHtml || '<div class="muted">No projects yet. Create one below!</div>';

    document.getElementById("reminder-count").textContent = `${reminders.length} reminders`;
    renderList(
      document.getElementById("reminder-list"),
      reminders,
      (r) => `
        <div>
          <div class="title">${r.title}</div>
          <div class="meta">⏰ ${r.time} · 🔄 ${r.recurrence} ${r.enabled ? "" : "⚠️ (disabled)"}</div>
          <div class="meta small">${r.message || "No message"} ${r.taskId ? " · 📎 Task #" + r.taskId : ""}</div>
        </div>
        <div class="controls">
          <button data-action="rem-delete" data-id="${r.id}">🗑️ Delete</button>
          <button data-action="rem-reschedule" data-id="${r.id}">📅 Reschedule</button>
          <button data-action="rem-edit" data-id="${r.id}">✏️ Edit</button>
          <span class="chip">#${r.id}</span>
        </div>
      `
    );

    await loadXPAndAchievements();
    await updatePomoState();
    await loadStatsSummary();
  } catch (err) {
    console.error(err);
    alert("Failed to load data: " + err.message);
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
    if (streakEl) streakEl.textContent = stats.streak || 0;
    if (pomosEl) pomosEl.textContent = stats.pomodoros || 0;
  } catch (e) {
    console.error("Failed to load stats summary:", e);
  }
}

async function loadXPAndAchievements() {
  try {
    const [xp, ach] = await Promise.all([
      fetchJSON("/api/xp"),
      fetchJSON("/api/achievements"),
    ]);
    cachedAchievements = ach;
    
    const percent = xp.next > 0 ? Math.min(100, (xp.xp / xp.next) * 100) : 100;
    const bar = document.getElementById("xp-bar");
    if (bar) bar.style.width = `${percent}%`;
    const inlineBar = document.getElementById("xp-bar-inline");
    if (inlineBar) inlineBar.style.width = `${percent}%`;

    const levelText = `Level ${xp.level} · ${xp.title}`;
    const progressText = `${xp.xp}/${xp.next} XP`;
    const pill = document.getElementById("xp-level-pill");
    if (pill) pill.textContent = `Level ${xp.level}`;
    const levelEl = document.getElementById("xp-level-text");
    if (levelEl) levelEl.textContent = levelText;
    const progEl = document.getElementById("xp-progress-text");
    if (progEl) progEl.textContent = `${progressText} to next`;
    const titleEl = document.getElementById("xp-title-text");
    if (titleEl) titleEl.textContent = `Title: ${xp.title}`;
    const inlineText = document.getElementById("xp-inline-text");
    if (inlineText) inlineText.textContent = levelText + " · " + progressText;
    document.getElementById("xp-box").textContent =
      `${levelText}\nXP: ${progressText}`;

    const achList = document.getElementById("achievements-list");
    const achCount = document.getElementById("ach-count");
    if (achCount) achCount.textContent = `${ach.length} tracked`;
    if (achList) {
      achList.innerHTML = ach
        .map(
          (a) => `
            <div class="ach-item" data-ach-id="${a.id}">
              <div class="title">${a.unlocked ? '🏆' : '🔒'} ${a.name || ("Achievement #"+a.id)}</div>
              <div class="percent">${(a.percent || 0).toFixed(1)}%</div>
              <div class="muted micro">${a.progress}/${a.target} progress · ${a.unlocked ? "✅ Unlocked" : "⏳ Locked"}</div>
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
            `<span class="ach-chip">${a.unlocked ? '🏆' : '🔒'} ${a.name || ("#"+a.id)} · ${(a.percent || 0).toFixed(0)}%</span>`
        )
        .join("");
    }
    document.getElementById("ach-box").textContent = ach
      .map((a) => `${a.unlocked ? '🏆' : '🔒'} ${a.name || ("#"+a.id)}: ${(a.percent || 0).toFixed(1)}% (${a.progress}/${a.target})`)
      .join("\n");
  } catch (e) {
    document.getElementById("xp-box").textContent = "XP load failed";
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
  if (!data.time || !isValidDateTimeStrict(data.time)) {
    alert("Invalid time. Please pick a valid date and time using the date picker.");
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
      await post(`/api/tasks/complete?id=${id}`);
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
    } else if (action === "pomo-work") {
      await post("/api/pomodoro/start");
      startPomoTimer(25, "Work Session");
    } else if (action === "pomo-break") {
      await post("/api/pomodoro/break");
      startPomoTimer(5, "Short Break");
    } else if (action === "pomo-long") {
      await post("/api/pomodoro/longbreak");
      startPomoTimer(15, "Long Break");
    } else if (action === "pomo-stop") {
      await post("/api/pomodoro/stop");
      stopPomoTimer();
    } else if (btn.dataset.stat) {
      const kind = btn.dataset.stat;
      const res = await fetchJSON(`/api/stats/${kind}`);
      document.getElementById("stat-box").textContent = res.report || res.heatmap || JSON.stringify(res, null, 2);
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
  pomoEndTime = Date.now() + minutes * 60 * 1000;
  
  const timerEl = document.getElementById("pomo-timer");
  const modeEl = document.getElementById("pomo-mode");
  const progressBar = document.getElementById("pomo-progress-bar");
  const statusEl = document.getElementById("pomo-status");
  
  if (modeEl) modeEl.textContent = mode;
  if (statusEl) statusEl.textContent = `${mode} started - ${minutes} minutes`;
  
  const totalMs = minutes * 60 * 1000;
  
  pomoTimer = setInterval(() => {
    const remaining = Math.max(0, pomoEndTime - Date.now());
    const mins = Math.floor(remaining / 60000);
    const secs = Math.floor((remaining % 60000) / 1000);
    
    if (timerEl) timerEl.textContent = `${pad(mins)}:${pad(secs)}`;
    
    const progress = ((totalMs - remaining) / totalMs) * 100;
    if (progressBar) progressBar.style.width = `${progress}%`;
    
    if (remaining <= 0) {
      stopPomoTimer();
      if (timerEl) timerEl.textContent = "00:00";
      if (modeEl) modeEl.textContent = "✅ " + mode + " Complete!";
      if (statusEl) statusEl.textContent = mode + " completed!";
      // Play notification sound or show alert
      try {
        new Notification("Pomodoro", { body: mode + " completed!" });
      } catch (e) {
        // Notifications might not be supported
      }
    }
  }, 1000);
}

function stopPomoTimer() {
  if (pomoTimer) {
    clearInterval(pomoTimer);
    pomoTimer = null;
  }
  const timerEl = document.getElementById("pomo-timer");
  const modeEl = document.getElementById("pomo-mode");
  const progressBar = document.getElementById("pomo-progress-bar");
  const statusEl = document.getElementById("pomo-status");
  
  if (timerEl) timerEl.textContent = "--:--";
  if (modeEl) modeEl.textContent = "Ready to start";
  if (progressBar) progressBar.style.width = "0%";
  if (statusEl) statusEl.textContent = "Pomodoro stopped.";
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

// Request notification permission on load
if ("Notification" in window && Notification.permission === "default") {
  Notification.requestPermission();
}

setupNavigation();
load();
