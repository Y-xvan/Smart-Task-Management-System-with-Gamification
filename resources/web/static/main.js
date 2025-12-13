const DATE_REGEX = /^\d{4}-\d{2}-\d{2}$/;
const DATETIME_REGEX = /^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$/;
let cachedProjects = [];
let cachedTasks = [];
let cachedReminders = [];

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
        <div>
          <div class="title">${t.name}</div>
          <div class="meta">${t.projectName ? `📁 ${t.projectName}` : "No project"} · Priority ${t.priority} · ${t.completed ? "✅ done" : "⏳ todo"}</div>
          <div class="meta small">Tags: ${t.tags || "none"} · Estimate: ${t.estimated || 0} pomodoro</div>
        </div>
        <div class="controls">
          <button data-action="complete" data-id="${t.id}">Complete</button>
          <button data-action="delete" data-id="${t.id}">Delete</button>
          <button data-action="assign" data-id="${t.id}">Assign</button>
          <button data-action="edit-task" data-id="${t.id}">Edit</button>
          <span class="chip">${t.due || "No due"}</span>
        </div>
      `
    );

    document.getElementById("project-count").textContent = `${projects.length} projects`;
    renderList(
      document.getElementById("project-list"),
      projects,
      (p) => `
        <div class="project-head" style="border-left-color:${p.color || "#4CAF50"}">
          <div class="title">${p.name}</div>
          <div class="meta">${p.description || "No description"}</div>
          <div class="meta small">Progress ${(p.progress * 100).toFixed(0)}% · Target ${p.target || "N/A"}</div>
          <div class="meta small">Tasks: ${(p.tasks || []).map(t => `${t.completed ? "✅" : "⬜"} ${t.name}${t.due ? " · " + t.due : ""}`).join("; ") || "No tasks yet"}</div>
        </div>
        <div class="controls">
          <button data-action="proj-delete" data-id="${p.id}">Delete</button>
          <button data-action="proj-update" data-id="${p.id}">Edit</button>
          <span class="chip" style="background:${p.color || "#4CAF50"}">${p.color || "#4CAF50"}</span>
        </div>
      `
    );

    document.getElementById("reminder-count").textContent = `${reminders.length} reminders`;
    renderList(
      document.getElementById("reminder-list"),
      reminders,
      (r) => `
        <div>
          <div class="title">${r.title}</div>
          <div class="meta">${r.time} · ${r.recurrence} ${r.enabled ? "" : "(disabled)"}</div>
          <div class="meta small">${r.message || "No message"} ${r.taskId ? " · Task #" + r.taskId : ""}</div>
        </div>
        <div class="controls">
          <button data-action="rem-delete" data-id="${r.id}">Delete</button>
          <button data-action="rem-reschedule" data-id="${r.id}">Reschedule</button>
          <button data-action="rem-edit" data-id="${r.id}">Edit</button>
          <span class="chip">#${r.id}</span>
        </div>
      `
    );

    await loadXPAndAchievements();
    await updatePomoState();
  } catch (err) {
    console.error(err);
    alert("Failed to load data: " + err.message);
  }
}

async function loadXPAndAchievements() {
  try {
    const [xp, ach] = await Promise.all([
      fetchJSON("/api/xp"),
      fetchJSON("/api/achievements"),
    ]);
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
            <div class="ach-item">
              <div class="title">${a.name || ("#"+a.id)}</div>
              <div class="percent">${(a.percent || 0).toFixed(1)}%</div>
              <div class="muted micro">${a.progress}/${a.target} progress · ${a.unlocked ? "Unlocked" : "Locked"}</div>
              <div class="muted micro">${a.description || ""}</div>
            </div>
          `
        )
        .join("");
    }
    const achInline = document.getElementById("achievements-inline");
    if (achInline) {
      achInline.innerHTML = ach
        .slice(0, 3)
        .map(
          (a) =>
            `<span class="ach-chip">${a.name || ("#"+a.id)} · ${(a.percent || 0).toFixed(0)}%</span>`
        )
        .join("");
    }
    document.getElementById("ach-box").textContent = ach
      .map((a) => `${a.name || ("#"+a.id)}: ${(a.percent || 0).toFixed(1)}% (${a.progress}/${a.target})`)
      .join("\n");
  } catch (e) {
    document.getElementById("xp-box").textContent = "XP load failed";
  }
}

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
  if (!isValidDateTimeStrict(data.time)) {
    alert("Invalid time. Please pick a real calendar time.");
    return;
  }
  const qs = new URLSearchParams(data).toString();
  await post("/api/reminders/create?" + qs);
  await load();
  e.target.reset();
});

document.getElementById("achievement-form").addEventListener("submit", async (e) => {
  e.preventDefault();
  const data = cleanEmptyFields(Object.fromEntries(new FormData(e.target).entries()));
  const qs = new URLSearchParams(data).toString();
  await post("/api/achievements/update?" + qs);
  await loadXPAndAchievements();
  e.target.reset();
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
      await post(`/api/projects/delete?id=${id}`);
    } else if (action === "proj-update") {
      const current = cachedProjects.find(p => String(p.id) === String(id));
      const name = prompt("Name?", current?.name || "");
      const desc = prompt("Description?", current?.description || "");
      const color = prompt("Color (hex)", current?.color || "#4CAF50") || current?.color || "#4CAF50";
      const target = prompt("Target date YYYY-MM-DD (blank keep)", current?.target || "") || "";
      if (target && !isValidDateStrict(target)) { alert("Invalid target date"); return; }
      const qs = new URLSearchParams(cleanEmptyFields({ id, name, desc, color, target })).toString();
      await post(`/api/projects/update?${qs}`);
    } else if (action === "rem-delete") {
      await post(`/api/reminders/delete?id=${id}`);
    } else if (action === "rem-reschedule") {
      const t = prompt("New time YYYY-MM-DD HH:MM:SS");
      if (!t) return;
      if (!isValidDateTimeStrict(t)) { alert("Invalid time format"); return; }
      await post(`/api/reminders/reschedule?id=${id}&time=${encodeURIComponent(t)}`);
    } else if (action === "rem-edit") {
      const current = cachedReminders.find(r => String(r.id) === String(id));
      const title = prompt("Title", current?.title || "") ?? current?.title ?? "";
      const message = prompt("Message", current?.message || "") ?? current?.message ?? "";
      const timeInput = prompt("Time YYYY-MM-DD HH:MM:SS (blank keep)", current?.time || "") || "";
      if (timeInput && !isValidDateTimeStrict(timeInput)) { alert("Invalid time"); return; }
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
      document.getElementById("pomo-status").textContent = "Work session started.";
    } else if (action === "pomo-break") {
      await post("/api/pomodoro/break");
      document.getElementById("pomo-status").textContent = "Short break started.";
    } else if (action === "pomo-long") {
      await post("/api/pomodoro/longbreak");
      document.getElementById("pomo-status").textContent = "Long break started.";
    } else if (action === "pomo-stop") {
      await post("/api/pomodoro/stop");
      document.getElementById("pomo-status").textContent = "Pomodoro stopped.";
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
    if (statusEl) {
      statusEl.textContent = `Running: ${state.running ? "Yes" : "No"} · Cycles: ${state.cycles}`;
    }
  } catch {
    const statusEl = document.getElementById("pomo-status");
    if (statusEl) statusEl.textContent = "Pomodoro state unavailable";
  }
}

setupNavigation();
load();
