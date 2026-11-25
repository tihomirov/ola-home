# server

Backend: users, homes, roles, device registry, REST API + WebSocket for the SPA,
and the MQTT side that talks to devices through Mosquitto.

Spec: [docs/spec/server.md](../docs/spec/server.md).

Stack (decided): **Python 3.13, FastAPI**, Uvicorn, Pydantic v2, SQLAlchemy 2 (async) +
asyncpg, Alembic, aiomqtt, argon2-cffi, pytest.

Not started yet (implementation order step 6).
