

**Keymancer** is a Windows tool that allows you to remap keyboard and mouse inputs based on a simple configuration file. It works globally by installing low-level input hooks and rerouting key/mouse events based on custom mappings.

---

## Features

- Remap any key or mouse button to another
- Works with both keyboard and mouse inputs
- Handles key up/down events for precise mapping
- Simple config format: `from--to` (e.g. `mouse1--mouse2`, `a--b`, `space--ctrl`)

---

## How It Works

- A low-level Windows hook intercepts all input events
- If remapping is enabled (`HOME` key toggles this), the hook checks if the input has a corresponding remap
- If a remap is found, the original input is swallowed and a synthetic input is sent using `SendInput`

---

## Notes

- This tool is designed for personal use and testing. Be cautious when using it in sensitive environments.