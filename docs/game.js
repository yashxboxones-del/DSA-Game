/* Browser port of the C++/SFML DSA platformer. */
const canvas = document.getElementById("game");
const ctx = canvas.getContext("2d");
const W = canvas.width, H = canvas.height;
const keys = new Set();
let state = "menu", menuIndex = 0, levelNo = 1, score = 0, coins = 0;
let player, level, checkpoints = [], checkpointStack = [], last = performance.now();

const levels = {
  1: {
    name: "Level 1 - Meadow Rise", spawn: [80, 540], goal: [100, 33],
    platforms: [[120,510,160,20],[340,430,160,20],[560,350,180,20],[800,275,160,20],
      [560,200,160,20],[320,135,180,20],[80,85,160,20]],
    coin: [[260,565],[190,475],[410,395],[640,315],[870,240],[630,165],[400,100],[115,50],[195,50]],
    enemy: [[450,564,360,720,85],[360,394,345,495,70],[600,314,565,735,75],[830,239,805,955,80],[350,99,325,495,85]],
    cp: [[640,350,"Plateau"],[400,135,"High Bridge"]]
  },
  2: {
    name: "Level 2 - Cavern Summit", spawn: [60,540], goal: [890,58],
    platforms: [[140,515,150,20],[360,440,150,20],[180,365,150,20],[390,290,160,20],
      [630,330,150,20],[850,260,150,20],[1060,190,160,20],[820,110,180,20]],
    coin: [[270,565],[210,480],[430,405],[250,330],[465,255],[700,295],[920,225],[1135,155],[860,75],[940,75]],
    enemy: [[500,564,350,780,95],[390,404,365,500,75],[210,329,185,320,80],[660,294,635,770,85],[880,224,855,990,90],[1090,154,1065,1210,95]],
    cp: [[465,290,"Center Cavern"],[1135,190,"Outer Spire"]]
  }
};

function rects(a,b) { return a.x < b.x+b.w && a.x+a.w > b.x && a.y < b.y+b.h && a.y+a.h > b.y; }
function loadLevel(n) {
  levelNo = n;
  const source = levels[n];
  level = {...source, platforms: source.platforms.map(p => [...p]), coin: source.coin.map(c => [...c]),
    enemy: source.enemy.map(e => [...e]), cp: source.cp.map(c => [...c])};
  player = {x:level.spawn[0], y:level.spawn[1], w:40, h:60, vx:0, vy:0, ground:false, hp:3, inv:0};
  checkpoints = level.cp.map(c => ({x:c[0], y:c[1], name:c[2], reached:false}));
  checkpointStack = [{x:player.x,y:player.y,hp:3,score}];
}
function startGame() { score = 0; coins = 0; loadLevel(1); state = "playing"; }
function pressed(...names) { return names.some(n => keys.has(n)); }
function processKey(k) {
    keys.add(k);
    if (state === "menu") {
      if (k === "arrowup" || k === "w") menuIndex = (menuIndex + 2) % 3;
      if (k === "arrowdown" || k === "s") menuIndex = (menuIndex + 1) % 3;
      if (k === "enter") menuIndex === 0 ? startGame() : menuIndex === 1 ? state = "controls" : (state = "menu");
      if (k === "escape" && state === "controls") state = "menu";
    } else if (state === "controls" && (k === "escape" || k === "enter")) state = "menu";
    else if (state === "playing") {
      if (k === "p" || k === "escape") state = "paused";
      if (k === "r") restoreCheckpoint();
      if (k === "u" && checkpointStack.length > 1) { checkpointStack.pop(); restoreCheckpoint(); }
    } else if (state === "paused") {
      if (k === "p" || k === "escape") state = "playing";
      if (k === "m") state = "menu";
    } else if (state === "gameover" || state === "win") {
      if (k === "enter") { if (state === "win") startGame(); else { state = "playing"; restoreCheckpoint(); } }
      if (k === "m") state = "menu";
    }
}
function handleKey(e) {
  const k = e.key.toLowerCase();
  if (["arrowup","arrowdown","arrowleft","arrowright"," ","escape"].includes(k)) e.preventDefault();
  if (e.type === "keydown") processKey(k); else keys.delete(k);
}
addEventListener("keydown", handleKey); addEventListener("keyup", handleKey);

document.querySelectorAll("[data-hold]").forEach(button => {
  const key = button.dataset.hold;
  const release = event => { event.preventDefault(); keys.delete(key); };
  button.addEventListener("pointerdown", event => {
    event.preventDefault(); keys.add(key); button.setPointerCapture(event.pointerId);
  });
  button.addEventListener("pointerup", release);
  button.addEventListener("pointercancel", release);
  button.addEventListener("pointerleave", release);
});
document.querySelectorAll("[data-key]").forEach(button => {
  const key = button.dataset.key;
  button.addEventListener("pointerdown", event => {
    event.preventDefault(); processKey(key);
    setTimeout(() => keys.delete(key), 0);
  });
});

function restoreCheckpoint() {
  const cp = checkpointStack[checkpointStack.length - 1];
  player.x = cp.x; player.y = cp.y; player.vx = player.vy = 0; player.hp = cp.hp; player.inv = 1.2;
  if (state === "gameover") state = "playing";
}
function update(dt) {
  if (state !== "playing") return;
  const left = pressed("a","arrowleft"), right = pressed("d","arrowright");
  player.vx = left !== right ? (left ? -300 : 300) : 0;
  if (pressed(" ","space") && player.ground) { player.vy = -520; player.ground = false; }
  player.inv = Math.max(0, player.inv - dt); player.vy += 1200 * dt;
  const oldBottom = player.y + player.h;
  player.x = Math.max(0, Math.min(W-player.w, player.x + player.vx*dt));
  player.y += player.vy*dt; player.ground = false;
  const surfaces = [[0,600,W,120], ...level.platforms];
  for (const p of surfaces) if (player.vy >= 0 && oldBottom <= p[1] + 4 &&
      player.y+player.h >= p[1] && player.x+player.w > p[0] && player.x < p[0]+p[2]) {
    player.y = p[1]-player.h; player.vy = 0; player.ground = true;
  }
  if (player.y > H+80) damage(3);
  for (const c of level.coin) if (!c.got && rects(player,{x:c[0]-10,y:c[1]-10,w:20,h:20})) { c.got=true; score+=100; coins++; }
  for (const e of level.enemy) {
    if (e.dead) continue;
    e.x += (e.dir || 1) * e[4] * dt; e.dir = e.dir || 1;
    if (e.x <= e[2]) e.x=e[2],e.dir=1; if (e.x+36>=e[3]) e.x=e[3]-36,e.dir=-1;
    const er={x:e.x,y:e[1],w:36,h:36};
    if (rects(player,er)) {
      if (player.vy>0 && player.y+player.h-er.y<24) { e.dead=true; player.vy=-360; score+=200; }
      else damage(1);
    }
  }
  for (const c of checkpoints) if (!c.reached && rects(player,{x:c.x-10,y:c.y-50,w:40,h:60})) {
    c.reached=true; checkpointStack.push({x:c.x,y:c.y-60,hp:player.hp,score});
  }
  const g={x:level.goal[0],y:level.goal[1],w:38,h:52};
  if (rects(player,g)) { if (levelNo === 1) loadLevel(2); else state="win"; }
}
function damage(n) { if (player.inv>0) return; player.hp=Math.max(0,player.hp-n); player.inv=1;
  if (!player.hp) state="gameover"; else restoreCheckpoint(); }

function text(s,x,y,size=24,color="#fff",align="center") { ctx.font=`bold ${size}px Arial`;ctx.fillStyle=color;ctx.textAlign=align;ctx.fillText(s,x,y); }
function draw() {
  const grad=ctx.createLinearGradient(0,0,0,H); grad.addColorStop(0,levelNo===2?"#16182b":"#224b79");grad.addColorStop(1,levelNo===2?"#252035":"#77c7c8");
  ctx.fillStyle=grad;ctx.fillRect(0,0,W,H);
  if (state==="menu"||state==="controls") return drawMenu();
  for (const p of level.platforms) {ctx.fillStyle=levelNo===2?"#414b63":"#82522d";ctx.fillRect(...p);ctx.strokeStyle="#d7a564";ctx.strokeRect(...p);}
  ctx.fillStyle="#2e7d32";ctx.fillRect(0,600,W,120);
  for (const c of checkpoints) {ctx.fillStyle="#ccc";ctx.fillRect(c.x, c.y-35,4,35);ctx.fillStyle=c.reached?"#28dcb4":"#78788c";ctx.fillRect(c.x+4,c.y-35,16,12);}
  ctx.fillStyle="#ffd750";ctx.fillRect(level.goal[0],level.goal[1],38,52);ctx.strokeStyle="#fff0a0";ctx.strokeRect(level.goal[0]-4,level.goal[1]-4,46,56);
  for (const c of level.coin) if (!c.got) {ctx.fillStyle="#ffe34d";ctx.beginPath();ctx.arc(c[0],c[1]+Math.sin(performance.now()/250+c[0]) * 3,10,0,Math.PI*2);ctx.fill();}
  for (const e of level.enemy) if (!e.dead) {ctx.fillStyle="#dc3030";ctx.fillRect(e.x,e[1],36,36);ctx.fillStyle="#ffe650";ctx.fillRect(e.x+6,e[1]+8,6,6);ctx.fillRect(e.x+22,e[1]+8,6,6);}
  if (!(player.inv>0 && Math.floor(player.inv*10)%2===0)) {ctx.fillStyle="#41b4f5";ctx.fillRect(player.x,player.y,player.w,player.h);ctx.strokeStyle="#fff";ctx.strokeRect(player.x,player.y,player.w,player.h);}
  text(`${level.name}   Score: ${score}   Coins: ${coins}   Health: ${"♥".repeat(player.hp)}`,20,30,20,"#fff","left");
  text("A/D or arrows: Move   Space: Jump   P/Esc: Pause   R: Respawn   U: Undo",20,705,15,"#d2e6ff","left");
  if (state==="paused") overlay("PAUSED","Press P/Esc to resume or M for menu");
  if (state==="gameover") overlay("GAME OVER","Press Enter to respawn or M for menu");
  if (state==="win") overlay("VICTORY!","Press Enter to play again or M for menu");
}
function overlay(title,sub) {ctx.fillStyle="rgba(5,10,25,.78)";ctx.fillRect(0,0,W,H);text(title,W/2,300,56,"#ffe37a");text(sub,W/2,360,22,"#fff");}
function drawMenu() {
  ctx.fillStyle="rgba(8,15,35,.4)";ctx.fillRect(0,0,W,H);text("DSA GAME",W/2,170,64,"#ffe37a");text("2D Platformer Adventure",W/2,215,24,"#c5dbff");
  if(state==="controls"){text("CONTROLS",W/2,300,38,"#7ee7d0");["A/D or arrows  Move","Space  Jump","R  Restore checkpoint","U  Undo checkpoint","P / Esc  Pause"].forEach((s,i)=>text(s,W/2,355+i*38,22));text("Enter/Esc to return",W/2,590,18,"#b8c9ed");return;}
  ["PLAY GAME","CONTROLS","EXIT"].forEach((s,i)=>text((i===menuIndex?"> ":"  ")+s,W/2,330+i*55,28,i===menuIndex?"#7ee7d0":"#fff"));
  text("Use W/S or arrows, then Enter",W/2,570,18,"#b8c9ed");
}
function loop(now) { const dt=Math.min((now-last)/1000,.1);last=now;update(dt);draw();requestAnimationFrame(loop); }
loadLevel(1); requestAnimationFrame(loop);
