# Windows Security Log Events for User Behavior Analysis

## Authentication Events
- **4624** - Successful logon
- **4625** - Failed logon attempt
- **4634** - Account logged off
- **4647** - User initiated logoff
- **4648** - Logon using explicit credentials (RunAs)

## Account Management
- **4720** - User account created
- **4722** - User account enabled
- **4723** - User attempted to change password
- **4724** - Password reset attempt
- **4725** - User account disabled
- **4726** - User account deleted
- **4728** - User added to security-enabled global group
- **4732** - User added to security-enabled local group
- **4740** - User account locked out

## Privilege Use
- **4672** - Special privileges assigned to new logon
- **4673** - Privileged service called
- **4674** - Operation attempted on privileged object
- **4688** - New process created
- **4689** - Process terminated

## File/Object Access
- **4656** - Handle to object requested
- **4658** - Handle to object closed
- **4660** - Object deleted
- **4663** - Attempt made to access object
- **5140** - Network share accessed
- **5145** - Network share checked for access permissions

## Policy Changes
- **4719** - System audit policy changed
- **4739** - Domain policy changed
- **4817** - Auditing settings on object changed

## System Events
- **4608** - Windows starting up
- **4609** - Windows shutting down
- **4616** - System time changed
- **4697** - Service installed
- **4698** - Scheduled task created
- **4699** - Scheduled task deleted
- **4700** - Scheduled task enabled
- **4701** - Scheduled task disabled

## Security Group Changes
- **4727** - Security-enabled global group created
- **4731** - Security-enabled local group created
- **4735** - Security-enabled local group changed
- **4737** - Security-enabled global group changed

## Key Behavioral Indicators
- **Multiple 4625 events** - Brute force attempts
- **4624 Type 10** - Remote interactive logons
- **4688 with suspicious processes** - Malware execution
- **4648 events** - Lateral movement attempts
- **4740 patterns** - Account lockout abuse
- **5140/5145 anomalies** - Unusual file access patterns
- **Off-hours 4624 events** - After-hours access
- **4698/4700 events** - Persistence mechanisms


# Windows Security Events to CERT Dataset Behavioral Features

## Logon Activity Features
| Windows Event | CERT Feature | Description |
|---------------|--------------|-------------|
| 4624 | `logon_count` | Daily successful logons per user |
| 4624 | `after_hours_logon` | Logons outside business hours (boolean) |
| 4624 | `weekend_logon` | Weekend logon activity (boolean) |
| 4625 | `failed_logon_count` | Failed authentication attempts |
| 4634/4647 | `session_duration` | Time between logon/logoff events |
| 4648 | `runas_count` | Explicit credential usage frequency |

## Device/Network Access Features  
| Windows Event | CERT Feature | Description |
|---------------|--------------|-------------|
| 5140/5145 | `file_access_count` | Network share access frequency |
| 5140/5145 | `unique_shares_accessed` | Number of distinct shares accessed |
| 4624 Type 10 | `remote_logon_count` | Remote desktop connections |
| 4663 | `object_access_count` | File/folder access attempts |

## Process/System Features
| Windows Event | CERT Feature | Description |
|---------------|--------------|-------------|
| 4688 | `process_count` | New processes created per day |
| 4688 | `suspicious_process` | Execution of flagged executables (boolean) |
| 4697 | `service_install` | Service installation events (boolean) |
| 4698/4700 | `scheduled_task` | Task scheduler modifications (boolean) |

## Administrative Activity Features
| Windows Event | CERT Feature | Description |
|---------------|--------------|-------------|
| 4672 | `admin_privilege_use` | Special privilege assignments |
| 4720/4726 | `account_management` | User account creation/deletion |
| 4728/4732 | `group_modification` | Security group membership changes |
| 4719/4739 | `policy_change` | System/domain policy modifications |

## Anomaly Detection Features
| Derived from Events | CERT Feature | Description |
|-------------------|--------------|-------------|
| 4624 patterns | `logon_deviation` | Deviation from normal logon times |
| 4688 analysis | `rare_process` | Execution of uncommon processes (boolean) |
| 5140 patterns | `data_volume` | Amount of data accessed/transferred |
| Multiple events | `activity_burst` | Sudden increase in user activity |
| 4625→4624 sequence | `brute_force_success` | Failed attempts followed by success |

## Temporal Features
| Time-based Analysis | CERT Feature | Description |
|-------------------|--------------|-------------|
| Event frequency | `hourly_activity` | Activity level per hour |
| Working patterns | `work_pattern_deviation` | Deviation from normal schedule |
| Event sequences | `unusual_sequence` | Atypical event ordering |

## Feature Engineering Examples
```
# Logon anomaly detection
logon_hour_deviation = |current_logon_hour - avg_logon_hour|
after_hours_flag = logon_hour < 8 OR logon_hour > 18

# Access pattern analysis  
unique_resources_ratio = unique_shares_today / avg_unique_shares
access_volume_spike = today_file_access > (avg_access * 3)

# Privilege escalation indicators
admin_events_increase = admin_actions_today > historical_avg * 2
new_privilege_use = first_time_special_privilege == TRUE
```