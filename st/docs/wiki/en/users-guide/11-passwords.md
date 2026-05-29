---
title: "Passwords"
lang: en
manual: users-guide-en
order: 11
---

# Passwords
SmartTar enforces access control through passwords. The system requires login before access-controlled operations can be performed.

## Access Levels Summary

| Level | What operators at this level can do |
|----|----|
| User 1 / User 2 | Monitor booths, settle calls, print receipts, view statistics. |
| Operator | View-only access to extension account information. Cannot change settings. |
| Supervisor | All of the above, plus: change configuration, manage turns, edit tariffs, manage passwords. |
| Backdoor | Full access for service technicians. Bypasses version checks. |

## Logging In

When an access-controlled function is invoked, SmartTar prompts for a password. Enter the password for the appropriate level and press **Enter**. The system grants access if the password matches any level with sufficient privileges.

## Changing Passwords

Go to **Configuración \> Claves**. Enter the old password, the new password, and confirm the new password. The new password takes effect immediately.

**Password rules:**

- Maximum 8 characters.
- Case-sensitive.
- Stored encrypted in ST.CFG.

**Note:** If you lock yourself out, contact your service technician. The backdoor password is used only by MicroDiseño service personnel.
