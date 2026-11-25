# web

SPA to manage homes, rooms, members and devices, and to control devices live.
Built from capabilities: each capability type maps to a widget (`brightness` →
slider, `switch` → toggle, `timer` → presets), so new devices need no web changes.

Spec: [docs/spec/web.md](../docs/spec/web.md) (screens, CRUD, widgets, live
updates, add-device wizard, security, project structure).

Uses REST for reading and changing data and WebSocket for live state. API types are
generated from the backend's OpenAPI.

Stack (decided): **React 19 + TypeScript**, Vite, TanStack Query/Router, Tailwind +
Radix (shadcn/ui style, restyled later from design mockups), i18next with
**English and Ukrainian**.

Implementation is step 7. It can start before the backend by using the mock API (`npm run dev:mock`).
