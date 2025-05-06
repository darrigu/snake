const game = document.getElementById('game');
const ctx = game.getContext('2d');

const front_fill_rect = (x, y, w, h, color) => {
  r = ((color >> (0 * 8)) & 0xFF).toString(16).padStart(2, '0');
  g = ((color >> (1 * 8)) & 0xFF).toString(16).padStart(2, '0');
  b = ((color >> (2 * 8)) & 0xFF).toString(16).padStart(2, '0');
  ctx.fillStyle = "#" + r + g + b;
  ctx.fillRect(x, y, w, h);
};

const wasm = WebAssembly.instantiateStreaming(fetch('back.wasm'), {
  env: {
    front_fill_rect,
  }
});

wasm.then((w) => {
  game.width = w.instance.exports.game_width();
  game.height = w.instance.exports.game_height();
  w.instance.exports.game_render();
});
