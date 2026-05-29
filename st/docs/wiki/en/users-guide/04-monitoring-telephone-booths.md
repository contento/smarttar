---
title: "Monitoring Telephone Booths"
lang: en
manual: users-guide-en
order: 4
---

# Monitoring Telephone Booths
SmartTar monitors all booths continuously without any operator action required. The booth view updates in real time as calls progress through their lifecycle.

## Booth States

| State | What it means |
|----|----|
| Libre (Idle) | Handset on hook. Booth is ready for use. |
| Descolgado | Handset lifted; waiting for digit input. |
| Marcando | Digits being dialed. |
| Llamando | Dialing complete; waiting for answer. |
| Hablando | Call connected and billing in progress. Cost accumulates in real time. |
| Colgado | Handset replaced; receipt being generated and printed. |
| Bloqueado | Post-hangup lock period; booth will return to idle shortly. |
| Error COM | Communication error count exceeded the configured threshold. |
| Error Marcación | Dialing error count exceeded the configured threshold. |

## Spy (Intervention) Mode

Supervisors can monitor a call in progress by selecting a booth and activating the spy function. While in spy mode, the operator can listen to the call without disturbing the customer. Spy mode is subject to the EXCLUSIVE_SPY configuration flag, which can limit simultaneous interventions to one booth at a time.

To activate spy mode: select the active booth cell and press the spy toolbar button, or use the keyboard shortcut defined in the resource file.

**Note:** Spy mode is available only to operators with Supervisor-level access. If NO_SOUND_WHILE_SPY is enabled, the alarm sound is suppressed during intervention.

## Alarm Conditions

The following alarm conditions trigger a visual indicator and, depending on configuration, an audible alert:

- **Communication error.** The booth could not establish a communication signal after dialing. Triggered when N_COM_ERR exceeds MAX_COM_ERR.
- **Dial error.** An invalid or incomplete digit sequence was received. Triggered when N_DIAL_ERR exceeds MAX_DIAL_ERR.
- **Incoming call detected.** When DETECT_INCOME is enabled, an incoming call on a booth line is flagged.
- **Inter-cluster error.** An unexpected signal from a booth in another cluster.

To clear an alarm: address the underlying cause and then use the alarm-reset function from the toolbar or the Configuration menu.
