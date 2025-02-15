
## Test SIGINT (Ctrl+C) while the Shell is **Idle**

### Goal
1. At the `minishell$` prompt, just press **Ctrl+C**.
2. Observe the behavior:
   - It **shouldn’t** in a Bash-like shell.
   - It produce a new prompt on a fresh line 
   - If you do `echo $?` afterward in Bash, you might see `130`, 

---

## Test SIGINT (Ctrl+C) **While a Child Process Is Running**

### Goal
1. Type a **long-running command** (e.g., `sleep 10`) :
2. Immediately press **Ctrl+C**.
3. Observe:
   - The `sleep 10` command should terminate right away.
   - minishell should show the prompt again (not exit).
   - `echo $?` right afterward would be `130`

---

## Test SIGQUIT (Ctrl+\)

### Goal
In Bash-like shells, pressing **Ctrl+\** at the idle prompt **often does nothing** (ignored). 
But if a child is running in the foreground, 
sending **SIGQUIT** typically kills it with a core dump (exit code `131`).

1. **Idle** test:
   - At `minishell$`, press **Ctrl+\**.
   - In Bash, usually nothing happens (the shell ignores SIGQUIT). 
   Check your shell: does it stay alive and just reprint the prompt or do nothing?

2. **Foreground child** test:
   - Type `sleep 10` again at your minishell prompt.
   - Press **Ctrl+\**.
   - Typically, that kills the child and prints something like “Quit (core dumped)” in Bash. 
   - The child is killed with exit code `131` (`128 + 3`) .  
   - The parent shell should remain alive.

---

## Verify the Exit Status (If Implemented)
<!-- 
If your shell stores the last command’s exit status in something like `$?`, 
you can test it by running something like:

1. **After** you kill `sleep 10` with Ctrl+C:
   ```bash
   minishell$ echo $?
   ```
   or however you retrieve your last status in your mini-shell. 
   - You might expect **130**.

2. **After** you kill `sleep 10` with Ctrl+\:
   ```bash
   minishell$ echo $?
   ```
   - You might expect **131**.
-->

---