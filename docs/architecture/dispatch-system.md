# Dispatch and callout lifecycle (first iteration)

The project-owned `police_dispatch` module implements deterministic callout publication and a guarded state machine:

\`Available → Accepted → OnScene → Completed\`

A non-terminal active callout can be cancelled. Only one callout may be accepted at a time. Arrival requires the player position to be within a caller-supplied radius. Completion awards the configured non-negative experience reward once and releases the active slot. Illegal transitions return false; malformed drafts throw \`std::invalid_argument\`.

This system currently runs as a headless C++ gameplay module and has unit tests. It is not yet connected to the in-game UI or PatrolScript host API, and callouts are not generated from a live traffic simulation.
