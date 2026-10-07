# Project Instructions

## Forbidden files

The following files are strictly off-limits:

- `src/secrets.h`

Codex, Claude and any other AI MUST NOT:
- open these files
- read their contents
- inspect their values
- modify them
- include their contents in responses
- infer or expose credentials stored inside them

If information from one of these files is required, ask the user to provide only the specific non-sensitive information needed.

## General rule

Never access credentials, API keys, passwords, tokens, or other secrets unless the user explicitly requests it for a specific task.