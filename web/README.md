# web

SPA to list, CRUD and control devices. Generic over capabilities via a
capability → widget map (`brightness` → slider, `switch` → toggle, `timer` → preset picker).

Uses REST for CRUD and WebSocket for live state. Types are generated from
[`../protocol/schemas`](../protocol/schemas).

**Open decision:** React or Vue. Implementation is step 7.
