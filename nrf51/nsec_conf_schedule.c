//
//  nsec_conf_schedule.c
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2016-05-13.
//
//

#include "nsec_conf_schedule.h"
#include "menu.h"
#include "ssd1306.h"
#include "controls.h"
#include "app_glue.h"
#include <stdlib.h>

void nsec_schedule_show_talks(uint8_t item);
void nsec_schedule_show_details(uint8_t item);

static menu_item_s days_schedule_items[] = {
    {
        .label = "Thursday May 19th '16",
        .handler = nsec_schedule_show_talks,
    }, {
        .label = "Friday, May 20th 2016",
        .handler = nsec_schedule_show_talks,
    },
};

static menu_item_s schedule_items_may_19[] = {
    {
        .label = "09:00 KEYNOTE : How Anonymous (narrowly) Evaded the Cyberterrorism Rhetorical Machine",
        .handler = nsec_schedule_show_details
    }, {
        .label = "10:00 Applying DevOps Principles for Better Malware Analysis",
        .handler = nsec_schedule_show_details
    }, {
        .label = "10:30 Stupid Pentester Tricks",
        .handler = nsec_schedule_show_details
    }, {
        .label = "11:00 The New Wave of Deserialization Bugs",
        .handler = nsec_schedule_show_details
    }, {
        .label = "13:30 Inter-VM Data Exfiltration: The Art of Cache Timing Covert Channel on x86 Multi-Core",
        .handler = nsec_schedule_show_details
    }, {
        .label = "14:30 Not Safe For Organizing: The state of targeted attacks against civil society",
        .handler = nsec_schedule_show_details
    }, {
        .label = "15:30 Practical Uses of Program Analysis: Automatic Exploit Generation",
        .handler = nsec_schedule_show_details
    }, {
        .label = "16:30 CANtact: An Open Tool for Automotive Exploitation",
        .handler = nsec_schedule_show_details
    }, {
        .label = "17:00 Security Problems of an Eleven Year Old and How to Solve Them",
        .handler = nsec_schedule_show_details
    }, {
        .label = "19:30 NorthSec Party",
        .handler = NULL
    },
};

static menu_item_s schedule_items_may_20[] = {
    {
        .label = "10:00 Bypassing Application Whitelisting in Critical Infrastructures",
        .handler = nsec_schedule_show_details
    }, {
        .label = "11:00 Law, Metaphor and the Encrypted Machine",
        .handler = nsec_schedule_show_details
    }, {
        .label = "13:30 Android - Practical Introduction into the (In)Security",
        .handler = nsec_schedule_show_details
    }, {
        .label = "14:30 Analysis of High-level Intermediate Representation in a Distributed Environment for Large Scale Malware Processing",
        .handler = nsec_schedule_show_details
    }, {
        .label = "15:30 Hide Yo' Kids: Hacking Your Family's Connected Things",
        .handler = nsec_schedule_show_details
    }, {
        .label = "16:30 Real Solutions From Real Incidents: Save Money and Your Job!",
        .handler = nsec_schedule_show_details
    }, {
        .label = "17:30 Conference Closing Speeches",
        .handler = nsec_schedule_show_details
    },
};

const char * presenters_may_19[] = {
    "Gabriella Coleman",
    "Olivier Bilodeau & Hugo Genesse",
    "Laurent Desaulniers",
    "Philippe Arteau",
    "Etienne Martineau",
    "Masashi Crete-Nishihata & John Scott-Railton",
    "Sophia D'Antoine",
    "Eric Evenchick",
    "Jake Sethi-Reiner",
};
const char * presenters_may_20[] = {
    "Ren\x82 Freingruber",
    "Lex Gill",
    "Miroslav Stampar",
    "Eugene Rodionov & Alexander Matrosov",
    "Mark Stanislav",
    "Guillaume Ross & Jordan Rogers",
};

const char * descriptions_may_19[] = {
    "Anonymous-the masked activists who have contributed to hundreds of political operations around the world since 2008–were perfectly positioned to earn the title of cyberterrorists. In this talk I consider the various factors that allowed them to narrowly escape this designation.",
    "The malware battle online is far from being over. Several thousands of new malware binaries are collected by antivirus companies every day. Most organizations don't have the expertise on staff to know if they are being targeted or if they are hit with mass-spreading malware, although knowing the difference is vital for a proper defensive strategy.",
    "Stumped in a pentest? You tried *everything* and yet have not been able to breach your target? \"Stupid Pentest Tricks\" presents several dirty tricks/cheats/ways to compromise your target in *creative ways*! Improve your ProxMark cloning skills, open doors using a universal RFID card, steal keys (no pickpocketing or impressioning skills needed), improve your phishing game and learn the mindset to cheat in a pentest. All this in a 30 minute talk.",
    "Recently, there have been several deserialization bugs released. In 2015, many Java softwares - including WebLogic, Jenkins and JBoss - were found vulnerable because of a common bug pattern. This talk will present the risk associated with deserialization mechanism and how it can be exploited. While a fix is available for some of the known vulnerable applications, your enterprise might be maintaining a proprietary application that is at risk.",
    "On x86 multi-core covert channels between co-located Virtual Machine (VM) are real and practical thanks to the architecture that has many imperfections in the way shared resources are isolated. This talk will demonstrate how a non-privileged application from one VM can ex-filtrate data or even establish a reverse shell into a co-located VM using a cache timing covert channel that is totally hidden from the standard access control mechanisms while being able to offer surprisingly high bps at a low error rate.",
    "Groups that work to protect human rights and civil liberties around the world are under attack by the many of the same attackers who target industry and government. These groups and organizations have far fewer resources to defend themselves, yet the stakes of the attacks are often much higher. This talk will give an update on the state of affairs, emphasizing two cases drawn from CItizen Lab's recent work: attacks against the Tibetan community, and the Packrat group in Latin America.",
    "Practical uses of program analysis will be presented and explained. Including Instrumentation, Symbolic and Concolic Execution, both in theory, in practice, and tools for each type. Specifically, this talk will show how to automatically generate an exploit against a complex, stand­alone application.",
    "Controller Area Network (CAN) remains the leading protocol for networking automotive controllers. Access to CAN gives an attacker the ability to modify system operation, perform diagnostic actions, and disable the system. CAN is also used in SCADA networks and industrial control systems. Historically, software and hardware for CAN has been expensive and targeted at automotive OEMs. Last year, we launched CANtact, an open source hardware CAN tool for PCs. This provides a low cost solution for converting CAN to USB and getting on the bus.",
    "This presentation will focus on the security problems faced by many eleven year olds, including protecting online accounts, securing your devices against siblings, circumventing parental restrictions, etc., and will present some potential solutions to these problems.",
};
const char * descriptions_may_20[] = {
    "Application whitelisting is a concept which can be used to further harden critical systems such as server systems in SCADA environments or client systems with high security requirements like administrative workstations. It works by whitelisting all installed software on a system and after that prevent the execution of not whitelisted software. This should prevent the execution of malware and therefore protect against advanced persistent threat (APT) attacks. In this talk we discuss the general security of such a concept and what holes are still open for attackers.",
    "Encryption technology raises unavoidable and ideologically loaded problems for courts—as recent cases like the FBI v Apple debate have bluntly illustrated. This tension has meant a real risk of shortsighted policy decisions that both jeopardize our civil liberties and compromise commercial interests. We all have a stake in the outcome of these debates, but the legal arguments are normally murky… at best. Judges reason through analogy and metaphor, using conceptual bridges to transition between old and new technologies in the law. But when new technologies inherit old metaphors, they also inherit old rules, models and limitations. So how do courts and lawmakers think about the encrypted machine—and how should they?",
    "This presentation covers the user's deadly sins of Android (In)Security, together with implied system security problems. Each topic could potentially introduce unrecoverable damage from security perspective. Both local and remote attacks are covered, along with accompanying practical demo of most interesting ones.",
    "Malware is acknowledged as an important threat and the number of new samples grows at an absurd pace. Additionally, targeted and so called advanced malware became the rule, not the exception. At Black Hat 2015 in Las Vegas the researchers co-authored a work on distributed reverse engineering techniques, using intermediate representation in a clustered environment. The results presented demonstrate different uses for this kind of approach, for example to find algorithmic commonalities between malware families. As a result, a rich dataset of metadata of 2 million malware samples was generated.",
    "This presentation will cover security research on Internet-connected devices targeting usage by, or for, children. Mark will discuss the vulnerabilities he found during this research, including account takeovers, device hijacking, backdoor credentials, unauthorized file downloading, and dangerously out-of-date protocols & software. Devices discussed will include Internet-connected baby monitors, a GPS-enabled platform to track children, and even a Wi-Fi & Bluetooth-connected stuffed animal.",
    "This talk will cover scenarios from real incidents and how simple solutions that are very cost effective can be used to prevent them from occurring. A scenario based on real incidents will be presented. The typical state of security in enterprise will be presented. Specific gaps that allowed the incident to occur and for data to be exfiltrated will be scrutinized.",
};

struct schedule_day_s {
    menu_item_s * menu_items;
    const char ** presenters;
    const char ** descriptions;
    uint8_t item_count;
};

struct schedule_day_s nsec_schedule[] = {
    {
        .menu_items = schedule_items_may_19,
        .presenters = presenters_may_19,
        .descriptions = descriptions_may_19,
        .item_count = sizeof(schedule_items_may_19) / sizeof(schedule_items_may_19[0]),
    }, {
        .menu_items = schedule_items_may_20,
        .presenters = presenters_may_20,
        .descriptions = descriptions_may_20,
        .item_count = sizeof(schedule_items_may_20) / sizeof(schedule_items_may_20[0]),
    },


};

enum schedule_state {
    SCHEDULE_STATE_CLOSED,
    SCHEDULE_STATE_DATES,
    SCHEDULE_STATE_TALKS,
    SCHEDULE_STATE_TALK_DETAILS,
};

static enum schedule_state schedule_state = SCHEDULE_STATE_CLOSED;
static uint8_t date_selected = 0;

void nsec_schedule_show_talks(uint8_t date);

static void nsec_schedule_button_handler(button_t button) {
    if(schedule_state == SCHEDULE_STATE_TALK_DETAILS && button != BUTTON_ENTER) {
        schedule_state = SCHEDULE_STATE_TALKS;
        menu_ui_redraw_all();
    }
    else if(button == BUTTON_BACK) {
        switch (schedule_state) {
            case SCHEDULE_STATE_TALKS:
                nsec_schedule_show_dates();
                break;
            case SCHEDULE_STATE_DATES:
                schedule_state = SCHEDULE_STATE_CLOSED;
                show_main_menu();
                break;

            default:
                break;
        }
    }
}

void nsec_schedule_show_dates(void) {
    menu_init(0, 8, 128, 56, sizeof(days_schedule_items) / sizeof(days_schedule_items[0]), days_schedule_items);
    schedule_state = SCHEDULE_STATE_DATES;
    nsec_controls_add_handler(nsec_schedule_button_handler);
}

void nsec_schedule_show_talks(uint8_t item) {
    date_selected = item;
    menu_init(0, 8, 128, 56, nsec_schedule[item].item_count, nsec_schedule[item].menu_items);
    schedule_state = SCHEDULE_STATE_TALKS;
}

void _nsec_schedule_show_details(uint8_t day, uint8_t item) {
    gfx_fillRect(0, 8, 128, 56, BLACK);
    gfx_setCursor(0, 8);
    gfx_setTextBackgroundColor(WHITE, BLACK);
    gfx_puts(nsec_schedule[day].menu_items[item].label);
    gfx_puts("\n");
    gfx_setTextBackgroundColor(BLACK, WHITE);
    gfx_puts((char *) nsec_schedule[day].presenters[item]);
    gfx_puts("\n");
    gfx_setTextBackgroundColor(WHITE, BLACK);
    gfx_puts((char *) nsec_schedule[day].descriptions[item]);
    gfx_update();
    schedule_state = SCHEDULE_STATE_TALK_DETAILS;
}

void nsec_schedule_show_details(uint8_t item) {
    if(item < nsec_schedule[date_selected].item_count - 1) {
        _nsec_schedule_show_details(date_selected, item);
    }
}
