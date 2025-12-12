async function fetchJSON(url) {
  const r = await fetch(url);
  if (!r.ok) throw new Error(r.statusText);
  return r.json();
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

    document.getElementById("task-count").textContent = `${tasks.length} items`;
    renderList(
      document.getElementById("task-list"),
      tasks,
      (t) => `
        <div>
          <div class="title">${t.name}</div>
          <div class="meta">Priority ${t.priority} · ${t.completed ? "✅ done" : "⏳ todo"}</div>
        </div>
        <span class="chip">${t.due || "No due"}</span>
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
        <span class="chip">${p.id}</span>
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
        <span class="chip">#${r.id}</span>
      `
    );
  } catch (err) {
    console.error(err);
    alert("Failed to load data: " + err.message);
  }
}

load();
