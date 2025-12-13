const DATE_REGEX = /^\d{4}-\d{2}-\d{2}$/;
const DATETIME_REGEX = /^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$/;
let cachedProjects = [];
let cachedTasks = [];

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
          <div class="meta">Priority ${t.priority} · ${t.completed ? "✅ done" : "⏳ todo"}</div>
        </div>
        <div class="controls">
          <button data-action="complete" data-id="${t.id}">Complete</button>
          <button data-action="delete" data-id="${t.id}">Delete</button>
          <button data-action="assign" data-id="${t.id}">Assign</button>
          <span class="chip">${t.due || "No due"}</span>
        </div>
      `
    );

    document.getElementById("project-count").textContent = `${projects.length} projects`;
    renderList(
      document.getElementById("project-list"),
      projects,
      (p) => `
        <div>
          <div class="title">${p.name}</div>
          <div class="meta">Progress ${(p.progress * 100).toFixed(0)}%</div>
        </div>
        <div class="controls">
          <button data-action="proj-delete" data-id="${p.id}">Delete</button>
          <button data-action="proj-update" data-id="${p.id}">Update</button>
          <span class="chip">#${p.id}</span>
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
          <div class="meta">${r.time} · ${r.recurrence}</div>
        </div>
        <div class="controls">
          <button data-action="rem-delete" data-id="${r.id}">Delete</button>
          <button data-action="rem-reschedule" data-id="${r.id}">Reschedule</button>
          <span class="chip">#${r.id}</span>
        </div>
      `
    );

    await loadXPAndAchievements();
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
              <div class="title">#${a.id}</div>
              <div class="percent">${(a.percent || 0).toFixed(1)}%</div>
              <div class="muted micro">${a.progress}/${a.target} progress</div>
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
            `<span class="ach-chip">#${a.id} · ${(a.percent || 0).toFixed(0)}%</span>`
        )
        .join("");
    }
    document.getElementById("ach-box").textContent = ach
      .map((a) => `#${a.id}: ${(a.percent || 0).toFixed(1)}% (${a.progress}/${a.target})`)
      .join("\n");
  } catch (e) {
    document.getElementById("xp-box").textContent = "XP load failed";
  }
}

// Forms
document.getElementById("task-form").addEventListener("submit", async (e) => {
  e.preventDefault();
  const data = cleanEmptyFields(Object.fromEntries(new FormData(e.target).entries()));
  if (data.due && !DATE_REGEX.test(data.due)) {
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
  if (data.target && !DATE_REGEX.test(data.target)) {
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
  if (!DATETIME_REGEX.test(data.time)) {
    alert("Invalid time. Use YYYY-MM-DD HH:MM:SS");
    return;
  }
  const qs = new URLSearchParams(data).toString();
  await post("/api/reminders/create?" + qs);
  await load();
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
    } else if (action === "proj-delete") {
      await post(`/api/projects/delete?id=${id}`);
    } else if (action === "proj-update") {
      const name = prompt("Name?", cachedProjects.find(p=>p.id==id)?.name || "");
      const desc = prompt("Description?", cachedProjects.find(p=>p.id==id)?.desc || "");
      await post(`/api/projects/update?id=${id}&name=${encodeURIComponent(name || "")}&desc=${encodeURIComponent(desc || "")}`);
    } else if (action === "rem-delete") {
      await post(`/api/reminders/delete?id=${id}`);
    } else if (action === "rem-reschedule") {
      const t = prompt("New time YYYY-MM-DD HH:MM:SS");
      if (!t) return;
      if (!DATETIME_REGEX.test(t)) { alert("Invalid time format"); return; }
      await post(`/api/reminders/reschedule?id=${id}&time=${encodeURIComponent(t)}`);
    } else if (action === "pomo-work") {
      await post("/api/pomodoro/start");
    } else if (action === "pomo-break") {
      await post("/api/pomodoro/break");
    } else if (action === "pomo-long") {
      await post("/api/pomodoro/longbreak");
    } else if (action === "pomo-stop") {
      await post("/api/pomodoro/stop");
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

load();
