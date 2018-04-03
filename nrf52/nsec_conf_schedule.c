//  Copyright (c) 2018
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

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
        .label = "Thursday May 18th 2017",
        .handler = nsec_schedule_show_talks,
    }, {
        .label = "Friday, May 19th 2017",
        .handler = nsec_schedule_show_talks,
    }, {
        .label = "Workshops",
        .handler = nsec_schedule_show_talks,
    }
};

static menu_item_s schedule_items_may_18[] = {
    {
        .label = "09:00 KEYNOTE - Playing Through the Pain: The Impact of Dark Knowledge and Secrets on Security and Intelligence Professionals",
        .handler = nsec_schedule_show_details
    }, {
        .label = "10:00 Attacking Linux/Moose Unraveled an Ego Market",
        .handler = nsec_schedule_show_details
    }, {
        .label = "11:00 Backslash Powered Scanning: Implementing Human Intuition",
        .handler = nsec_schedule_show_details
    }, {
        .label = "13:30 Deep Dive into Tor Onion Services",
        .handler = nsec_schedule_show_details
    }, {
        .label = "14:30 BearSSL: SSL For all Things",
        .handler = nsec_schedule_show_details
    }, {
        .label = "15:30 Data Science Tools and Techniques for the Blue Team",
        .handler = nsec_schedule_show_details
    }, {
        .label = "16:30 Murder Mystery - How Vulnerability Intelligence is Poisoning your Information Security Program",
        .handler = nsec_schedule_show_details
    }, {
        .label = "17:30 Dinner Break",
        .handler = NULL
    }, {
        .label = "19:30 NorthSec Party!",
        .handler = NULL
    },
};

static menu_item_s schedule_items_may_19[] = {
    {
        .label = "09:30 How Surveillance Law was Expanded in Canada, What the Media has Reported, and What's Next",
        .handler = nsec_schedule_show_details
    }, {
        .label = "10:00 Creating an Internet of (Private) Things - Some Things for Your Smart Toaster to Think About",
        .handler = nsec_schedule_show_details
    }, {
        .label = "10:30 Don't Kill My Cat",
        .handler = nsec_schedule_show_details
    }, {
        .label = "11:00 Modern Reconnaissance Phase by APT - Protection Layer",
        .handler = nsec_schedule_show_details
    }, {
        .label = "13:30 Stupid RedTeamer Tricks",
        .handler = nsec_schedule_show_details
    }, {
        .label = "14:00 Abusing Webhooks for Command and Control",
        .handler = nsec_schedule_show_details
    }, {
        .label = "14:30 Pentesting: Lessons from Star Wars",
        .handler = nsec_schedule_show_details
    }, {
        .label = "15:30 Hacking POS PoS Systems",
        .handler = nsec_schedule_show_details
    }, {
        .label = "16:30 Hack Microsoft Using Microsoft Signed Binaries",
        .handler = nsec_schedule_show_details
    }, {
        .label = "17:30 Closing Speeches",
        .handler = NULL
    },
};

static menu_item_s schedule_items_workshops[] = {
    {
        .label = "Thursday",
        .handler = NULL
    }, {
        .label = "10:00 Introduction to Assembly Language and Shellcoding",
        .handler = nsec_schedule_show_details
    }, {
        .label = "14:30 Automating Detection, Investigation and Mitigation with LimaCharlie",
        .handler = nsec_schedule_show_details
    }, {
        .label = "Friday",
        .handler = NULL
    }, {
        .label = "10:00 Cracking Custom Encryption - An Intuitive Approach to Uncovering Malware's Protected Data",
        .handler = nsec_schedule_show_details
    }, {
        .label = "14:30 Script Engine Hacking For Fun And Profit",
        .handler = nsec_schedule_show_details
    }
};

const char * presenters_may_18[] = {
    "Richard Thieme",
    "Masarah Paquet-Clouston & Olivier Bilodeau",
    "James Kettle",
    "David Goulet",
    "Thomas Pornin",
    "Shawn Marriott",
    "Gordon MacKay",
};
const char * presenters_may_19[] = {
    "Chris Prince",
    "Ian Douglas",
    "Charles F. Hamilton",
    "Paul Rascagneres",
    "Laurent Desaulniers",
    "Dimitry Snezhkov",
    "Adam Shostack",
    "Jackson Thuraisamy & Jason Tran",
    "Pierre-Alexandre Braeken",
};
const char * presenters_workshops[] = {
    NULL,
    "Charles F. Hamilton & Peter Heppenstall",
    "Maxime Lamothe-Brassard",
    NULL,
    "Pavel Asinovsky & Magal Baz",
    "Jean-Marc Le Blanc & Israel Halle",
};

const char * descriptions_may_18[] = {
    "Dismissing or laughing off concerns about what it does to a person to know critical secrets does not lessen the impact on life, work, and relationships of building a different map of reality than \"normal people\" use. One has to calibrate narratives to what another believes. One has to live defensively, warily. This causes at the least cognitive dissonance which some manage by denial. But refusing to feel the pain does not make it go away. It just intensifies the consequences when they erupt.",
    "For this talk, a criminologist and a security researcher teamed up to hunt a large-scale botnet dubbed Linux/Moose that conducts social media fraud. Linux/Moose has stealth features and runs only on embedded systems such as consumer routers or Internet of Things (IoT) devices. Using honeypots set up across the world, we managed to get virtual routers infected to learn how this botnet spread and operated. We performed a large-scale HTTPS man-in-the-middle attack on several honeypots over the course of several months decrypting the bots' proxy traffic. This gave us an impressive amount of information on the botnet's activities on social networks: the name of the fake accounts it uses, its modus operandi to conduct social media fraud and the identification of its consumers, companies and individuals.",
    "Existing web scanners search for server-side injection vulnerabilities by throwing a canned list of technology-specific payloads at a target and looking for signatures - almost like an anti-virus. In November I released an open-source scanner that takes an alternative approach, capable of finding and confirming both known and unknown classes of injection vulnerabilities. Evolved from classic manual techniques, this approach reaps many associated benefits including casual WAF evasion, a tiny network footprint, and flexibility in the face of input filtering.",
    "Millions of people around the world use Tor every day to protect themselves from surveillance and censorship. While most people use Tor to reach ordinary websites more safely, a tiny fraction of Tor traffic makes up what overhyped journalists like to call the \"Dark Web\". Tor onion services (formerly known as Tor hidden services) let people run Internet services such as websites in a way where both the service and the people reaching it can get stronger security and privacy.",
    "BearSSL is a novel SSL/TLS library optimised for constrained systems, aiming at small code footprint and low RAM usage. The talk is about presenting the library in its context, and delving into what makes a good SSL implementation and how BearSSL does it.",
    "Every year organizations generate more data, and security teams are expected to make sense of not just a greater volume of data from the myriad of log sources that exist in corporate environments, but new sources of logs and data as well. In this talk we look at the data scientist methodology and some of the statistical and machine learning techniques available to defenders of corporate infrastructure. After explaining the strengths and weaknesses of the different techniques we will walk through analyzing some data and spend some time explaining the python code and what would be needed to scale the code from analyzing hundreds of thousands of data points to tens of millions. This is not a talk about SIEM, and related technologies. SIEM is good at collecting logs to a central location and performing on the fly inspection and correlations, but rarely has the ability to engage in deeper statistical analysis, or employ machine learning techniques.",
    "Integrating vulnerability scanning results into one's security ecosystem involves a serious hidden challenge which results in heinous consequences, thereby killing your InfoSec program. This session shares clues on this challenge, step by step, in the form of a murder mystery game, and ultimately reveals the culprit as well as strategies to overcome it. Come participate, play, and interact! Try to guess \"who-dunnit,\" and learn how to avoid future similar InfoSec crimes.",
};
const char * descriptions_may_19[] = {
    "This talk will provide an overview on the specific lawful access powers that came into force in Canada March 2015; how they are rolling out in the view of the media and the courts (e.g. the TELUS and Rogers cases), and; how the authorities intersection with S-4 and C-51 (around permissions for information-sharing). Some highlights from the recent submission on rights and security around lawful access, encryption and hacking tools will also be covered.",
    "The next big market push is to have the cool IoT device that's connected to the internet. As we've seen from the Mirai and Switcher hacks, it's important to embed the appropriate safeguards so that devices are not open to attack. When selecting device components there are things that should be checked for, and when you're doing the coding and workflows, there are other things that need to be taken in to account. Although security and privacy are close cousins, they're also different. This talk will be centered around some best security and privacy practices as well as some common errors that should be avoided.",
    "The purpose of this presentation is to introduce a tool and the idea behind it. This tool evades antivirus, sandboxes, IDS/IPS using one simple technique. In a nutshell it abuses of polyglot files and compact low level obfuscation using assembly. The target system can then execute the payload using various vectors: powershell or Windows' executable.",
    "The Talos researchers are no stranger to APT attacks. During recent research, we observed how APT actors are evolving and how the reconnaissance phase is changing to protect their valuable 0-day exploit or malware frameworks. During the presentation, we will not speak about a specific malware actor but we will use various different cases to illustrate how the reconnaissance phase is becoming more important and more complex. This talk will mainly focus on the usage of malicious documents (Microsoft Office and Hangul Word Processor) and watering hole attacks designed to establish if the target is the intended one. We will mention campaigns against political or military organizations targeting USA, Europa and Asia.",
    "Who said that you need to be elite to be a good red teamer? This presentation focuses on simple, easy hacks that can change the result of a red team assessment. The 30 minute talk will cover improvements on the age old classic of dropping usb keys (35% increase in payload delivery!); how to reduce your C&C discoverabiltiy; techniques for leveraging Outlook against your victim to improve social engineering and other very simple tricks. By the end of the presentation, audience should be inspired to build upon techniques discussed in the talk and feel more confident in doing red team engagements.",
    "You are on the inside of the perimeter. And maybe you want to exfiltrate data, download a tool, or execute commands on your command and control server (C2). Problem is - the first leg of connectivity to your C2 is denied. Your DNS and ICMP traffic is being monitored. Access to your cloud drives is restricted. You've implemented domain fronting for your C2 only to discover it is ranked low by the content proxy, which is only allowing access to a handful of business related websites on the outside.",
    "Everyone knows you ought to threat model, but in practical reality it turns out to be tricky. If past efforts to threat model haven't panned out, perhaps part of the problem is confusion over what works, and how the various approaches conflict or align. This talk captures lessons from years of work helping people throughout the software industry threat model more effectively. It's designed to help security pros, especially pen testers, all of whom will leave with both threat modeling lessons from Star Wars and a proven foundation, enabling them to threat model effectively on offense or defense.",
    "Hackers try to find the easiest ways to achieve the most impact. When it comes to credit card fraud, compromising Point of Sale (PoS) systems is the latest trend. The presenters will share their experience on how attackers can exploit both technical and policy gaps to breach organizations. This talk will cover approaches to physical security, kiosk breakouts, and the extraction of sensitive data. It's laced with real-life examples, including a detailed discussion of recently disclosed critical vulnerabilities in Oracle's hotel management platform.",
    "Imagine being attacked by legitimate software tools that cannot be detected by usual defender tools. How bad could it be to be attacked by malicious threat actors only sending bytes to be read and bytes to be written in order to achieve advanced attacks? The most dangerous threat is the one you can't see. At a time when it is not obvious to detect memory attacks using API like VirtualAlloc, what would be worse than having to detect something like \"f 0xffffe001`0c79ebe8+0x8 L4 0xe8 0xcb 0x04 0x10\"? We will be able to demonstrate that we can achieve every kind of attacks you can imagine using only PowerShell and a Microsoft Signed Debugger. We can retrieve passwords from the userland memory, execute shellcode by dynamically parsing loaded PE or attack the kernel achieving advanced persistence inside any system.",
};
const char * descriptions_workshops[] = {
    NULL,
    "The purpose of this workshop is to familiarize participants with assembly language. At the end of the workshop, participants will be able to understand shellcode and optimize it to avoid null bytes or blacklisted characters.",
    "The workshop will begin with an overview of the various detection and automation mechanisms available in LimaCharlie.",
    NULL,
    "As banking fraud researchers, we take part in a never-ending chase after new configurations of banking malware. We strive to have the upper hand, by figuring out where the configurations are hidden and how they are encrypted.",
    "More and more applications are allowing execution of untrusted code in their context to extend themselves. Whether it’s Javascript in a web browser, Lua plugins in video game or ruby to customize business rules, it is important to keep your infrastructure secure when running them.",
};

struct schedule_day_s {
    menu_item_s * menu_items;
    const char ** presenters;
    const char ** descriptions;
    uint8_t item_count;
};

struct schedule_day_s nsec_schedule[] = {
    {
        .menu_items = schedule_items_may_18,
        .presenters = presenters_may_18,
        .descriptions = descriptions_may_18,
        .item_count = sizeof(schedule_items_may_18) / sizeof(schedule_items_may_18[0]),
    }, {
        .menu_items = schedule_items_may_19,
        .presenters = presenters_may_19,
        .descriptions = descriptions_may_19,
        .item_count = sizeof(schedule_items_may_19) / sizeof(schedule_items_may_19[0]),
    }, {
        .menu_items = schedule_items_workshops,
        .presenters = presenters_workshops,
        .descriptions = descriptions_workshops,
        .item_count = sizeof(schedule_items_workshops) / sizeof(schedule_items_workshops[0]),
    }
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
    gfx_fillRect(0, 8, 128, 56, SSD1306_BLACK);
    gfx_setCursor(0, 8);
    gfx_setTextBackgroundColor(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts(nsec_schedule[day].menu_items[item].label);
    gfx_puts("\n");
    gfx_setTextBackgroundColor(SSD1306_BLACK, SSD1306_WHITE);
    gfx_puts((char *) nsec_schedule[day].presenters[item]);
    gfx_puts("\n");
    gfx_setTextBackgroundColor(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts((char *) nsec_schedule[day].descriptions[item]);
    gfx_update();
    schedule_state = SCHEDULE_STATE_TALK_DETAILS;
}

void nsec_schedule_show_details(uint8_t item) {
    if(item < nsec_schedule[date_selected].item_count) {
        _nsec_schedule_show_details(date_selected, item);
    }
}
