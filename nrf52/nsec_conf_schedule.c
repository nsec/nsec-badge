//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <nordic_common.h>
#include <stdlib.h>
#include <string.h>
#include "nsec_conf_schedule.h"
#include "menu.h"
#include "ssd1306.h"
#include "controls.h"
#include "app_glue.h"

#define ROW_COUNT                   7 // 8 - status bar
#define COLUMN_COUNT                21
#define MAX_CHAR_UNDER_STATUS_BAR   COLUMN_COUNT * ROW_COUNT

static void nsec_schedule_button_handler(button_t button);
void nsec_schedule_show_talks(uint8_t item);
void nsec_schedule_show_details(uint8_t item);
void nsec_schedule_show_tracks(uint8_t item);
void nsec_schedule_show_party(uint8_t item);
void nsec_schedule_show_presenters(uint8_t item);

static menu_item_s days_schedule_items[] = {
    {
        .label = "Thursday May 17th 2018",
        .handler = nsec_schedule_show_tracks
    }, {
        .label = "Friday, May 18th 2018",
        .handler = nsec_schedule_show_tracks
    }, {
        .label = "Presenters",
        .handler = nsec_schedule_show_presenters
    }
};

static menu_item_s tracks_schedule_items[] = {
    {
        .label = "Track 1",
        .handler = nsec_schedule_show_talks
    }, {
        .label = "Track 2",
        .handler = nsec_schedule_show_talks
    }, {
        .label = "Workshop",
        .handler = nsec_schedule_show_talks
    }
};

static menu_item_s schedule_items_thursday_track_1[] = {
    {
        .label = "09:00 KEYNOTE - How to Think (About Complex Adversarial Systems)",
        .handler = nsec_schedule_show_details
    }, {
        .label = "10:00 Break",
        .handler = NULL
    }, {
        .label = "10:15 What are containers exactly and can they be trusted?",
        .handler = nsec_schedule_show_details
    }, {
        .label = "11:15 Break",
        .handler = NULL
    }, {
        .label = "11:30 Ichthyology: Phishing as a Science",
        .handler = nsec_schedule_show_details
    }, {
        .label = "12:00 Lunch break",
        .handler = NULL
    },{
        .label = "13:15 Non-Crypto Constant-Time Coding",
        .handler = nsec_schedule_show_details
    }, {
        .label = "13:45 Break",
        .handler = NULL
    }, {
        .label = "14:00 Logic against sneak obfuscated malware",
        .handler = nsec_schedule_show_details
    }, {
        .label = "14:30 Break",
        .handler = NULL
    }, {
        .label = "14:45 Exploits in Wetware",
        .handler = nsec_schedule_show_details
    }, {
        .label = "15:45 Break",
        .handler = NULL
    }, {
        .label = "16:00 Binary analysis, meet the blockchain",
        .handler = nsec_schedule_show_details
    }, {
        .label = "17:00 Break",
        .handler = NULL
    }, {
        .label = "17:15 Data Breaches: Barbarians in the Throne Room",
        .handler = nsec_schedule_show_details
    }, {
        .label = "20:00+ NorthSec Party!",
        .handler = nsec_schedule_show_party
    },
};

static menu_item_s schedule_items_thursday_track_2[] = {
    {
        .label = "09:00 KEYNOTE - ????",
        .handler = nsec_schedule_show_details
    }, {
        .label = "10:00 Break",
        .handler = NULL
    }, {
        .label = "10:15 Getting ahead of the elliptic curve",
        .handler = nsec_schedule_show_details
    }, {
        .label = "11:15 Break",
        .handler = NULL
    }, {
        .label = "11:30 From Hacking Team to Hacked Team to...?",
        .handler = nsec_schedule_show_details
    }, {
        .label = "12:00 Lunch break",
        .handler = NULL
    }, {
        .label = "13:15 Stupid Purple Teamer Tricks",
        .handler = nsec_schedule_show_details
    }, {
        .label = "13:45 Break",
        .handler = NULL
    }, {
        .label = "14:00 Prototype pollution attacks in NodeJS applications",
        .handler = nsec_schedule_show_details
    }, {
        .label = "14:30 Break",
        .handler = NULL
    }, {
        .label = "14:45 Brain Implants & Mind Reading",
        .handler = nsec_schedule_show_details
    }, {
        .label = "15:45 Break",
        .handler = NULL
    }, {
        .label = "16:00 Quick Retooling with .NET Payloads",
        .handler = nsec_schedule_show_details
    }, {
        .label = "17:00 Break",
        .handler = NULL
    }, {
        .label = "17:15 The Blackbear project",
        .handler = nsec_schedule_show_details
    }, {
        .label = "17:45 Browser Exploitation",
        .handler = nsec_schedule_show_details
    }, {
        .label = "20:00+ NorthSec Party!",
        .handler = nsec_schedule_show_party
    },
};

static menu_item_s schedule_items_thursday_workshops[] = {
    {
        .label = "Workshops 1",
        .handler = NULL
    }, {
        .label = "10:15 Hands-on Modern Access Control Bypassing",
        .handler = nsec_schedule_show_details
    }, {
        .label = "12:15 Lunch break",
        .handler = NULL
    }, {
        .label = "13:15 A Gentle Introduction to Fuzzing",
        .handler = nsec_schedule_show_details
    }, {
        .label = "Workshops 2",
        .handler = NULL
    }, {
        .label = "10:00 Capture-The-Flag 101",
        .handler = nsec_schedule_show_details
    }, {
        .label = "12:15 Lunch break",
        .handler = NULL
    }, {
        .label = "13:15 Incident Response in the Age of Threat Intelligence with MISP, TheHive & Cortex",
        .handler = nsec_schedule_show_details
    }
};

static menu_item_s schedule_items_friday_track_1[] = {
    {
        .label = "10:15 Video game hacks, cheats and glitches",
        .handler = nsec_schedule_show_details
    }, {
        .label = "11:15 Break",
        .handler = NULL
    }, {
        .label = "11:30 Only an Electron away from code execution",
        .handler = nsec_schedule_show_details
    }, {
        .label = "12:00 Lunch break",
        .handler = NULL
    }, {
        .label = "13:15 Smart contract vulnerabilities: The most interesting transactions on the Ethereum blockchain",
        .handler = nsec_schedule_show_details
    }, {
        .label = "14:15 Break",
        .handler = NULL
    }, {
        .label = "14:30 Tightening the Net in Iran",
        .handler = nsec_schedule_show_details
    }, {
        .label = "15:30 Break",
        .handler = NULL
    }, {
        .label = "15:45 Invoke-DOSfuscation: Techniques FOR %F IN (-style) DO (S-level CMD Obfuscation)",
        .handler = nsec_schedule_show_details
    }, {
        .label = "16:45 Break",
        .handler = NULL
    }, {
        .label = "17:00 Homeward Bound: Scanning private Ip Space with DNS Rebinding",
        .handler = nsec_schedule_show_details
    },
};

static menu_item_s schedule_items_friday_track_2[] = {
    {
        .label = "10:15 Python and Machine Learning: How to use algorithms to create yara rules with a malware zoo for hunting",
        .handler = nsec_schedule_show_details
    }, {
        .label = "11:15 Break",
        .handler = NULL
    }, {
        .label = "11:30 One Step Before Game Hackers -- Instrumenting Android Emulators",
        .handler = nsec_schedule_show_details
    }, {
        .label = "12:00 Lunch break",
        .handler = NULL
    }, {
        .label = "13:15 Not the Droid You're Looking For: Evading Vulnerability Exploitation Through Secure Android Development",
        .handler = nsec_schedule_show_details
    }, {
        .label = "14:15 Break",
        .handler = NULL
    }, {
        .label = "14:30 Surprise Supplies",
        .handler = nsec_schedule_show_details
    }, {
        .label = "15:30 Break",
        .handler = NULL
    }, {
        .label = "15:45 The Current State of IMSI-Catchers",
        .handler = nsec_schedule_show_details
    }, {
        .label = "16:45 Break",
        .handler = NULL
    }, {
        .label = "17:00 A Journey into Red Team",
        .handler = nsec_schedule_show_details
    },
};

static menu_item_s schedule_items_friday_workshops[] = {
    {
        .label = "Workshops 1",
        .handler = NULL
    }, {
        .label = "10:15 Wi-Fi Security",
        .handler = nsec_schedule_show_details
    }, {
        .label = "12:00 Lunch break",
        .handler = NULL
    }, {
        .label = "13:15 Botnet Tracking and Data Analysis Using Open-Source Tools",
        .handler = nsec_schedule_show_details
    }, {
        .label = "Workshops 2",
        .handler = NULL
    }, {
        .label = "10:00 Hacking APIs and the MEAN Stack with OWASP DevSlop",
        .handler = nsec_schedule_show_details
    }, {
        .label = "12:00 Lunch break",
        .handler = NULL
    }, {
        .label = "13:15 IoT Firmware Exploitation",
        .handler = nsec_schedule_show_details
    }
};

const char * presenters_thursday_track_1[] = {
    "Eleanor Saitta",
    NULL,
    "Stephane Graber",
    NULL,
    "Karla Burnett",
    NULL,
    "Thomas Pornin",
    NULL,
    "Thais Moreira Hamasaki",
    NULL,
    "Robert Sell",
    NULL,
    "Mark Mossberg",
    NULL,
    "Dave Lewis",
    NULL,
};

const char * presenters_thursday_track_2[] = {
    "Martijn Grooten",
    NULL,
    "Filip Kafka",
    NULL,
    "Laurent Desaulniers",
    NULL,
    "Olivier Arteau",
    NULL,
    "Melanie Segado",
    NULL,
    "Dimitry Snezhkov",
    NULL,
    "Marc-Andre Labonte",
    NULL,
    "Jean-Marc Leblanc",
    NULL,
};

const char * presenters_thursday_workshops[] = {
    NULL,
    "Vikram Salunke",
    NULL,
    "Jean-Marc Leblanc & Israel Halle",
    NULL,
    "Olivier Bilodeau, Laurent Desaulniers & Charles F. Hamilton",
    NULL,
    "Saad Khadi & Raphael Vinot",
};

const char * presenters_friday_track_1[] = {
    "Ron Bowes",
    NULL,
    "Silvia Valli",
    NULL,
    "Sarah Friend & Jon Maurelian",
    NULL,
    "Mahsa Alimardani",
    NULL,
    "Daniel Bohannon",
    NULL,
    "Allan Wirth & Danny Cooper",
};

const char * presenters_friday_track_2[] = {
    "Sebastien Larinier",
    NULL,
    "Wan Mengyuan",
    NULL,
    "Kristina Balaam",
    NULL,
    "Paul Rascagneres",
    NULL,
    "Yomna Nasser",
    NULL,
    "Charles F. Hamilton",
};

const char * presenters_friday_workshops[] = {
    NULL,
    "Mark El-Khoury",
    NULL,
    "Olivier Bilodeau & Masarah Paquet-Clouston",
    NULL,
    "Nicole Becher & Tanya Janca",
    NULL,
    "Aaron Guzman",
};

const char * descriptions_thursday_track_1[] = {
    "It's possible to approach security as a series of one-off technical problems to solve in series (from either the attacker or defender perspective). While this can often help you find and fix specific bugs, it's not particularly useful for either securing or attacking an organization at scale, and tends to fail badly when you attempt to interact with humans. Everyone who works in security finds patterns in their work, and scaling up and orchestrating interactions with those patterns is a large part of how we make progress. We rarely talk about the larger structures of these patterns, though, and, being of a practical bent, often try to turn back to practice too quickly -- hence much of e.g. the lackluster discourse around threat modeling. In this talk, I'll look into some of the things I've noticed about how to think that may be useful for security practitioners of all stripes.",
    NULL,
    "Everyone's talking about containers these days. But how many actually know what they are? Do you know there are two big families of containers and that even within those, there are countless different runtimes to set them up and manage them? Is a VM safer than a container? What about those containers that are using VM technology for containers? Those are all questions anyone who's dealing with containers in production should know answers to. You should be able to decide whether to use containers at all and if so, what kind of container is the best fit for your particular task. During this presentation, we'll be going over 15 years of container technologies on Linux (10 years in mainline Linux), how the security features they're built on top have evolved and what's the current state of things. We'll be comparing application containers to system containers, actual containers to lightweight virtualization and briefly cover some of the higher level management tools that come with them and what to keep in mind when trying to keep all of that safe.",
    NULL,
    "Many companies consider phishing inevitable: the best we can do is run training for our employees, and cross our fingers. But does phishing training actually work? In this talk we'll cover the psychology of phishing, then walk through a series of real-world attacks conducted against a Bay Area tech company - including conversion rates for each attack, and ways in which existing protections were bypassed. We'll cover recent technological advancements in this area, then combine these with our case studies to provide evidence-based techniques on how to prevent, not just mitigate, credential phishing.",
    NULL,
    "Cache attacks are a class of side-channel attacks that have been used since 2005 to break implementations of cryptographic algorithms. However, they do not impact only cryptography; if a given context makes cache attacks applicable, then everything that handles confidential data is potentially vulnerable. The SGX technology offers such a context where all the code in an enclave, not only its encryption code, shall be made robust to such attacks. In this talk, we present a summary of cache attacks, SGX, and a toolkit of C functions designed to help with writing generic, non-crypto, constant-time code.",
    NULL,
    "Malware is sneaky. Malicious codes are implemented to stay hidden during the infection and operation, preventing their removal and the analysis of the code. Most samples employ some sort of packing or obfuscation techniques in order to thwart analysis. Similar techniques are also used to protect digital assets from intellectual property theft. Analysis tools help getting new insights that can be used to secure software and hardware by identifying vulnerabilities and issues before they cause harm downstream. Tools and techniques beyond standard debuggers can enhance analysts capabilities with better adaptability and automation. This talk will give you a small taste on some practical applications of SMT solvers in IT security, investigating the theoretical limitations and practical solutions, focusing on their use as a tool for binary static analysis and code deobfuscation.",
    NULL,
    "Robert discusses his third place experience at the Defcon 2017 SE CTF and how his efforts clearly show how easy it is to get sensitive information from any organization. The 2017 Verizon report clearly shows the dramatic growth rate of social engineering attacks and Robert demonstrates how he collected hundreds of data points from the target organization using OSINT techniques. He then goes into the vishing strategy he implemented to maximize the points he collected in the 20 minute live contest. Without much effort Robert was able to know their VPN, OS, patch level, executive personal cell phone numbers and place of residence. Robert lifts the curtain of the social engineering world by showing tricks of the trade such as the “incorrect confirmation” which is one of many methods to loosen the tongues of his marks. Robert then shows the pretexts he designed to attack companies and the emotional response each pretext is designed to trigger. By knowing these patters we can better educate our staff. With that much information at his fingertips, how long would it take him to convince your executive to make a bank transfer? If your organization lost a few million dollars due to social engineering, who would be to blame? Are you insured for that? Who is getting fired? Robert wraps up his talk with a series of strategies companies can take to reduce exposure and risk. He goes over current exposure, building defenses, getting on the offense and finally… a culture shift",
    NULL,
    "Ethereum is a novel, decentralized computation platform that has quickly risen in popularity since it was introduced in 2014, and currently controls the equivalent of one hundred ten billion dollars. At its foundation is a virtual machine which executes “smart contracts”: programs that ultimately control the majority of the value transfer within the network. As with most other types of programs, correctness is very important for smart contracts. However, somewhat uniquely to Ethereum, incorrectness can have a direct financial cost, as evidenced by a variety of high profile attacks involving the loss of hundreds of millions of dollars. The error-prone nature of developing smart contracts and the increasing amounts of capital processed by them motivates the development of analysis tools to assist in automated error and vulnerability discovery. In this talk, we describe our work towards smart contract analysis tooling for Ethereum, which focuses on a modern technique called symbolic execution. We provide context around both Ethereum and symbolic execution, and then discuss the unique technical challenges involved with combining the two, touching on topics including blockchains, constraint solvers, and virtual machine internals. Lastly, we present Manticore: an open source symbolic execution tool which we have used to enhance smart contract security audits.",
    NULL,
    "Often defenders worry about the intangible security problems. Defenders need to concentrate their efforts defending the enterprise by focusing on the fundamentals. Too often issues such as patching or system configuration failures lead to system compromise. These along with issues such as SQL injection are preventable problems. Defenders can best protect their digital assets by first understanding the sheer magnitude that a data breach can have on an enterprise. In this talk I review my findings after analyzing hundreds of data breach disclosures as it pertains to what went wrong.",
    NULL,
};

const char * descriptions_thursday_track_2[] = {
    "Elliptic curves are relatively obscure mathematical objects: you can get a PhD in maths without ever having come across them. Yet these objects play an important role in modern cryptography and as such are found in most HTTPS connections, in Bitcoin, and in a large number of other places. To really understand elliptic curve cryptography (ECC) to the point that you can implement algorithms, you'd have to study the maths behind it. This talk assumes that you haven't studied the maths, but just want to understand what ECC is about, how is works and how it is implemented. It will discuss how 'point addition' works and how the Elliptic Curve Diffie-Hellman algorithm is used, for example in HTTPS - and how you can find it using Wireshark. It will explain how to use elliptic curve for digital signatures and why you don't want to be like Sony when it comes to implementing them. It will discuss how ECC was used in an infamous random number generator and, finally, will take a brief look at the use of elliptic curves in post-quantum algorithms. The goal of this talk is to keep things simple and understandable and no knowledge of maths is assumed. The talk won't make you an expert on ECC -- that would take years of studying. But it might help you understand the context a bit better when you come across them in your research. And hopefully it will also be a little bit fun.",
    NULL,
    "Hacking Team came into the spotlight of the security industry following its damaging data breach in July 2015. The leaked data revealed several 0-day exploits being used and sold to governments, and confirmed Hacking Team’s suspected business with oppressive regimes. But what happened to Hacking Team after one of the most famous hacks of recent years? Hacking Team’s flagship product, the Remote Control System (RCS), was detected in the wild in the beginning of 2018 in fourteen countries, including those contributing to previous criticism of the company’s practices. We will present the evidence that convinced us that the new post-hack Hacking Team samples can be traced back to a single group – not just any group – but Hacking Team’s developers themselves. Furthermore, we intend to share previously undisclosed insights into Hacking Team’s post-leak operations, including the targeting of diplomats in Africa, uncover digital certificates used to sign the malware, and share details of the distribution vectors used to target the victims. We will compare the functionality of the post-leak samples to that in the leaked source code. To help other security researchers we’ll provide tips on how to efficiently extract details from these newer VMProtect-packed RCS samples. Finally, we will show how Hacking Team sets up companies and purchases certificates for them.",
    NULL,
    "Stupid tricks for everyone! This talk will present very simple, low tech attacks to better achieve your goals, both attack and defense. From a defense standpoint, this talk will present simple tricks to identify Responder on your network, pinpoint BurpSuite activities, block some active crimeware and other simple tricks. Offensive tricks include a very simple NAC bypass, even more physical pentesting tricks and some very simple changes to social engineering that can help a lot.",
    NULL,
    "Prototype pollution is a term that was coined many years ago in the JavaScript community to designate libraries that added extension methods to the prototype of base objects like \"Object\", \"String\" or \"Function\". This was very rapidly considered a bad practice as it introduced unexpected behavior in applications. In this presentation, we will analyze the problem of prototype pollution from a different angle. What if an attacker could pollute the prototype of the base object with his own value? What APIs allow such pollution? What can be done with it?",
    NULL,
    "At a certain level of technological sophistication you gain the ability to grant others read, write, and execute permissions to your brain. That ability creates a unique set of security concerns. In this talk we will discuss the current state of both brain scanning and brain stimulation technology, the practical implications of merging brains with artificial intelligence, and the role infosec can play in shaping the dystopic cyberpunk future that we’re currently careening towards.",
    NULL,
    "PowerShell gave us a super-highway of convenient building blocks for offensive toolkits and operational automation. In the post offensive PowerShell world, a move in the direction of .NET implants may be a desirable option in some cases. However, Red Teams are faced with challenges when moving automation down into managed code. Can .NET based toolkits maintain flexibility, quick in-field retooling and operational security in the face of current detection mechanisms? We think the answer is yes. In this talk, we will focus on quick in-field retooling and dynamic execution aspect of .NET implants as the crucial trait to overcome static defensive mechanisms. We will dive deeper into OpSec lessons learned from dynamic code compilation. We will attempt to move beyond static nature of .NET assemblies, into reflective .NET DLR. We will showcase on-the-fly access to native Windows API and discuss methods of hiding sensitive aspects of execution in the managed code memory. All that, with the help of the DLRium Managed Execution toolkit we have in development.",
    NULL,
    "In typical enterprise networks today, ingress filtering is taken care of by firewall or similar devices. Unfortunately, the ability of devices and applications to reach the outside world is often overlooked or intentionnaly left open as Web services might need to be reacheable. We will present a fork of an OpenSSH daemon, that is able to exploit the often loose egress filtering and maneuver around network restrictions. Designed for more comfortable post-exploitation, it also extends regular forwarding and tunneling abilities in order to circumvent network rules that may otherwise hinder lateral movement. In addition, it can also act as a regular SSH server listening for an incoming connection, and provides reliable interactive shell access (must be able to run top, sudo, screen, vi, etc) as opposed to crafted reverse shells or even meterpreter which allow basic commands but fail at interactive ones.",
    NULL,
    "????"
};

const char * descriptions_thursday_workshops[] = {
    NULL,
    "This workshop will teach you how to attack applications secured by Firewalls, IDS/IPS, Antivirus, WAF. The presenter will describe the newest bypassing techniques and provide a systematic and practical approach on how to bypass modern access control mechanisms. This workshop contains lot of demos. Everyone is now using Firewalls, IDS/IPS, Load Balancers with multiple features such as algorithms, signatures etc. Since the beginning, filter obfuscation and evading technique have been there. These mechanisms provide multiple layers of defense, so bypassing them is an important aspect of pentesting. This workshop describes different techniques to bypass these mechanisms. We will see them in action with multiple demos. Just bring your laptop to learn these attacks practically.",
    NULL,
    "Fuzzing could be summed up as a testing method feeding random inputs to a program. Where a more traditional approach to testing relies on manual design of tests based on known assumptions, fuzzing bring an automated mean of creating test cases. Although a single test generated by a fuzzer is unlikely to find any defaults, millions of them in quick iterations makes it very likely to trigger unexpected behaviours and crashes. With the rise of smarter fuzzers, fuzzing has become an efficient and reliable way to test most edge cases of a program and makes it possible to cover very large programs that would require otherwise a large amount of effort from manual reviewing and testing. The low amount of manual intervention required to setup a modern smart fuzzer dismiss any pretexts a developer or security research might have to not fuzz its project. If you aren't fuzzing, the bad guys will (and find all the bugs that comes with it). This workshop aims to introduce the basic concepts of fuzzing to the participants and to enable them to make fuzzing a critical step of their testing process. The class is going to start with a quick introduction about the concepts of fuzzing, why they should do it and some benefits other organisations have gain from it. The workshop will then move on to an hands-on approach on how to set up AFL and run it against a program and how to interpret the outputs. Most of the exercice will turn around a sample program with intentional bugs and gotchas, and once the participants will have an understanding of the basis, they will be walked through real world scenarios. Finally, a time will be allocated at the end for the participants to fuzz a project of their choice with the assistance of the presenters.",
    NULL,
    "The objective of this workshop is to dive into Capture-The-Flag (CTF) competitions. First, by introducing them. Then by helping both individuals and teams prepare but also evolve in their practice of applied cybersecurity. We will have various levels (easy, medium, hard) of CTF challenges in several categories (binaries, exploitation, Web, forensics) and we will give hints and solutions during the workshop. This is meant to be for CTF first timers. Seasoned players should play NorthSec’s official CTF instead.",
    NULL,
    "The goal of the tutorial is to familiarize participants with Incident Response and Cyber Threat Intelligence using TheHive — a Security Incident Response Platform, Cortex — a powerful observable analysis engine, and MISP — the de facto standard platform for threat sharing.",
};

const char * descriptions_friday_track_1[] = {
    "Ron built his security career in a unique way: writing cheats for video games. In highschool, while others were having fun, he was trying to find new and creative ways to confuse Starcraft. In this presentation, he will look at some of the major hacks, cheats, and glitches in video games, from famous ones (like arbitrary code execution in Super Mario World) to obscure ones (like stacking buildings in Starcraft). But more importantly, he will tie these into modern vulnerabilities: the Legend of Zelda \"bottle glitch\" is a type-confusion vulnerability, for example: similar vulnerabilities in normal software could lead to remote code execution. This talk will bridge video game cheating with real-world security vulnerabilities, and explore the history of both!",
    NULL,
    "Over the decades, various security techniques to mitigate desktop specific vulnerabilities have been developed which makes it difficult to successfully exploit traditional desktop applications. With the rise of Electron framework, it became possible to develop multi-platform desktop applications by using commonly known web technologies. Developed by the Github team, Electron has already become amazingly popular (used by Skype, Slack, Wire, Wordpress and so many other big names), bringing adventurous web app developers to explore the desktop environment. These same developers who make the XSS to be the most common web vulnerability are now bringing the same mistakes to a whole new environment. While XSS in the web applications is bounded by the browser, the same does not apply to Electron applications. Making the same kind of mistakes in an Electron application widens the attack surface of desktop applications, where XSS can end up being so much more dangerous. So in this talk, I will discuss the Electron framework and the related security issues, its wonderful “features” getting me a bunch of CVE’s, possible attack vectors and the developers in the dark about these issues. AND as Electron apps do not like to play in the sandbox, this talk will DEMO Electron applications found to be vulnerable, gaining code execution from XSS.",
    NULL,
    "Smart contract security is a brave, new, and sometimes terrible field. This presentation will take you through some of the most famous vulnerabilities of these first few years (from the Dao hack, to the Parity wallet vulnerabilities ... and including less-well-known but very interesting events like the DDOS attacks from late 2016). We'll explain the details of how these attacks work(ed), some of the idiosyncrasies of Ethereum, and through these examples some general principles of smart contract security.",
    NULL,
    "How do Iranians experience the Internet? Various hurdles and risks exist for Iranians and including outside actors like American technology companies. This talk will assess the state of the Internet in Iran, discuss things like the threats of hacking from the Iranian cyber army; how the government are arresting Iranians for their online activities; the most recent policies and laws for censorship, surveillance and encryption; and the policies and relationships of foreign technology companies like Apple, Twitter and Telegram with Iran, and the ways they are affecting the everyday lives of Iranians. This talk will effectively map out how the Internet continues to be a tight and controlled space in Iran, and what efforts are being done and can be done to make the Iranian Internet a more accessible and secure space.",
    NULL,
    "Skilled attackers continually seek out new attack vectors and effective ways of obfuscating old techniques to evade detection. Active defenders can attest to attackers’ prolific obfuscation of JavaScript, VBScript and PowerShell payloads given the ample availability of obfuscation frameworks and their effectiveness at evading many of today’s defenses. However, advanced defenders are increasingly detecting this obfuscation with help from the data science community. This approach paired with deeper visibility into memory-resident payloads via interfaces like Microsoft’s Antimalware Scan Interface (AMSI) is causing some Red Teamers to shift tradecraft to languages that offer defenders less visibility. But what are attackers using in the wild? In the past year numerous APT and FIN (Financial) threat actors have increasingly introduced obfuscation techniques into their usage of native Windows binaries like wscript.exe, regsvr32.exe and cmd.exe. Some simple approaches entail randomly adding cmd.exe’s caret (^) escape character to command arguments. More interesting techniques like those employed by APT32, FIN7 and FIN8 involve quotes, parentheses and standard input.",
    NULL,
    "DNS Rebinding attacks have re-entered the spotlight, largely owing to recent high-profile disclosures by Tavis Ormandy including RCE in the Blizzard Update Agent triggered from the browser. However, given the vast amount of consumer software in circulation today and the apparent frequency with which the design (anti)pattern of treating localhost as secure occurs, it is likely that many vulnerable services still exist. In this talk, we will present a set of tools we created to make performing DNS Rebinding attacks fast and easy at scale, discuss how these tools can be used to perform network reconnaissance from inside a browser, and present an opt-in “localhost census” page that uses DNS rebinding to enumerate localhost services listening for HTTP on the visitor’s computer, and adds the results to a database.",
};

const char * descriptions_friday_track_2[] = {
    "Machine learning can be useful for helping analysts and reverse engineers. This presentation will explain how to transform data to use machine-learning algorithms to categorize a malware zoo. To cluster a set of (numerical) objects is to group them into meaningful categories. We want objects in the same group to be closer (or more similar) to each other than to those in other groups. Such groups of similar objects are called clusters. When data is labeled, this problem is called supervised clustering. It is a difficult problem but easier than the unsupervised clustering problem we have when data is not labeled. All our experiments have been done with code written in Python and we have mainly used scikit-learn. With the dataset the Zoo, we present how to use unsupervised algorithms on labeled datasets to validate the model. When the model is finalized, the resulting clusters can be used to automatically generate yara rules in order to hunt down the malware.",
    NULL,
    "Commercial Android emulators such as NOX, BlueStacks and Leidian are very popular at the moment and most games can run on these emulators fast and soundly. The bad news for game vendors is that these emulators are usually shipped with root permission in the first place. On the other hand, cheating tools developers are happy because they can easily distribute their tools to abusers without requiring the abusers to have a physical rooted device, nor do they need to perform trivial tuning for different Android OS / firmware version. However, luckily for game vendors, commercial Android emulators usually use an x86/ARM mixed-mode emulation for speed-up. As a result, a standard native hooking/DBI framework won't work on this kind of platform. This drawback could discourage the cheating developers. In this talk, I will introduce a native hooking framework on such a kind of mixed-mode emulators. The talk will include the process start routine of both command-line applications and Android JNI applications as well as how these routines differ on an emulator. The different emulation strategies adopted by different emulators and runtime environments (Dalvik/ART) will also be discussed. Based on these knowledge, I will explain why the existing hooking/DBI frameworks do not work on these emulators and how to make one that works. Lastly, I will present a demo of using this hooking framework to cheat a game on emulator. With this demo, I will discuss how the dark market of mobile game cheating may develop in the foreseeable future.",
    NULL,
    "The first commercially-available Android device was released in 2008. Despite its nearly 10-year public lifespan, the OS still poses numerous security challenges. Now, as mobile becomes an increasingly popular platform for consumers, we're faced with the challenge of protecting these consumers from new, quickly evolving threats. We’ll discuss why Android security is so much more challenging for software developers compared to iOS and the web, look at the most common attack vectors for the operating system, and walk through best practices for guarding against them.",
    NULL,
    "Supply chain attacks are often long thought about and often overlooked in terms of how well a business prepares itself for any associated compromise or breach. 2017 has truly marked itself as 'The Year Of The Supply Chain Attack' and marked a turning point concerning supply chain attacks. Talos was involved in two major campaigns: MeDoc compromise that paralyzed the Ukraine and CCleaner compromise that impacted a reported 2.27M consumers. In this presentation we will first present these two cases. In both cases, we will present how the attackers modified a legitimate application and what was the result of the modification. We will explain the purpose of the attackers and the malware used against the victims. For the MeDoc compromise, we were directly involved in the incident response and we will provide a timeline of the events to give an idea of the before, during and after picture associated with Nyetya and MeDoc. Concerning the CCleaner compromise, we will provide some data and statistics from the attacker's database and the profiles of the targeted organizations. In a second part, we will speak globally about supply chain attacks. We will remember that it's not the first time in the history that this kind of attacks occurred and we will finally open the discussion on the future of this attacks.",
    NULL,
    "IMSI-catchers, also known as cell-site simulators, are devices that let their operators track cell users, interfere with their calls/texts, and mount other privacy-invasive attacks. While research around IMSI-catchers has been gaining traction over the years, there hasn’t been much effort into making the more technical results accessible outside of academia and niche hardware hacking circles. The goal of this talk is to remedy that. This talk will be a deep technical dive into how cell networks interact with user equipment, the details of how IMSI-catchers exploit their design flaws, what goes into building an IMSI-catcher (hypothetically, of course!), the relationship Canadian & American law enforcement have with these devices, and steps one can take to protect themselves.",
    NULL,
    "This talk will describe many issues that a redteamer may face during a Red Team exercise. Being stealth is one of them; avoiding detection of your lateral movements, phishing campaign and post exploitation are crucial to succeed. Over the years I've developed tools and different approaches that can be used during standard engagement and Red Team to remain stealth and move more efficiently into your victim network. During the presentation several techniques will be described and analyzed to understand the idea behind them.",
};

const char * descriptions_friday_workshops[] = {
    NULL,
    "This workshop will briefly go over the Wi-Fi basics and known security issues, covering WPA2-Personal, WPA2-Enterprise, WPS, and then focusing on the most recent developments in Wi-Fi such as KRACK, and will include hands-on labs. The workshop will also cover direct attacks against wireless clients and access points, such as router vulnerabilities, rogue access points and denial-of-service attacks.",
    NULL,
    "Fully understanding a botnet often requires a researcher to go beyond standard reverse-engineering practices and explore the malware’s network traffic. The latter can provide meaningful information on the evolution of a malware’s activity. However, it is often disregarded in malware research due to time constraints and publication pressures. The workshop is about overcoming such constraints by providing a powerful workflow to conduct quick analysis of malicious traffic. The data science approach presented capitalizes on open-source tools (Wireshark/Tshark, Bash with GNU parallel) and valuable python libraries (ipython, mitmproxy, pandas, matplotlib). During the workshop, participants will do practical technical labs with datasets from our recent botnet investigation. They will learn how to quickly find patterns, plot graphs and interpret data in a meaningful way. Although the exercises will focus on botnet’s data, the tools and skills learned will be useful to all sorts of context. Moreover, to ensure that participants take the most out of the workshop, it will be built in a way to allow them to easily replicate the data-analysis environment at home and reproduce similar analysis with their own traffic data.",
    NULL,
    "Modern applications often use APIs and other micro services to deliver faster and better products and services. However, there are currently few training grounds for security testing in such areas. In comes DevSlop, OWASP's newest project, a collection of DevOps security disasters made as a vulnerable testing and proving ground for developers and security testers alike. DevSlop's Pixi, the first of many entries to come for this OWASP project, will be demonstrated and presenting for participant's hacking and learning pleasure. Pixi consists of vulnerable web services, and participants will be walked through how to exploit several of it's vulnerabilities so they can learn how to do better when they create their own web services and other types of APIs from now on.",
    NULL,
    "The IoT Firmware Exploitation and Attack Countermeasures workshop is designed to provide techniques for testing of embedded IoT systems, employing proactive controls, embedded application security best practices, and address the challenges of building security into embedded devices. This course is suited for embedded systems engineers, software developers, and security professionals of all backgrounds. Hands on demonstrations and labs will be given throughout the course.",
};

struct schedule_day_s {
    menu_item_s * menu_items;
    const char ** presenters;
    const char ** descriptions;
    uint8_t item_count;
};

struct schedule_day_s nsec_schedule[] = {
    {
        .menu_items = schedule_items_thursday_track_1,
        .presenters = presenters_thursday_track_1,
        .descriptions = descriptions_thursday_track_1,
        .item_count = ARRAY_SIZE(schedule_items_thursday_track_1),
    }, {
        .menu_items = schedule_items_thursday_track_2,
        .presenters = presenters_thursday_track_2,
        .descriptions = descriptions_thursday_track_2,
        .item_count = ARRAY_SIZE(schedule_items_thursday_track_2),
    }, {
        .menu_items = schedule_items_thursday_workshops,
        .presenters = presenters_thursday_workshops,
        .descriptions = descriptions_thursday_workshops,
        .item_count = ARRAY_SIZE(schedule_items_thursday_workshops),
    }, {
        .menu_items = schedule_items_friday_track_1,
        .presenters = presenters_friday_track_1,
        .descriptions = descriptions_friday_track_1,
        .item_count = ARRAY_SIZE(schedule_items_friday_track_1),
    }, {
        .menu_items = schedule_items_friday_track_2,
        .presenters = presenters_friday_track_2,
        .descriptions = descriptions_friday_track_2,
        .item_count = ARRAY_SIZE(schedule_items_friday_track_2),
    }, {
        .menu_items = schedule_items_friday_workshops,
        .presenters = presenters_friday_workshops,
        .descriptions = descriptions_friday_workshops,
        .item_count = ARRAY_SIZE(schedule_items_friday_workshops),
    },
};

enum schedule_state {
    SCHEDULE_STATE_CLOSED,
    SCHEDULE_STATE_DATES,
    SCHEDULE_STATE_TRACK,
    SCHEDULE_STATE_TALKS,
    SCHEDULE_STATE_TALK_DETAILS,
};

static enum schedule_state schedule_state = SCHEDULE_STATE_CLOSED;
static uint16_t description_index = 0;
static uint8_t date_selected = 0;
static uint8_t track_selected = 0;
static uint8_t talk_selected = 0;
static uint8_t schedule_index = 0; // Keep track of our index in the scheduler array

void nsec_schedule_show_dates(void) {
    menu_init(0, 8, 128, 56, ARRAY_SIZE(days_schedule_items), days_schedule_items);
    schedule_state = SCHEDULE_STATE_DATES;
    nsec_controls_add_handler(nsec_schedule_button_handler);
}

void nsec_schedule_show_talks(uint8_t item) {
    track_selected = item;
    schedule_index = (date_selected * 3) + track_selected;
    menu_init(0, 8, 128, 56, nsec_schedule[schedule_index].item_count, nsec_schedule[schedule_index].menu_items);
    schedule_state = SCHEDULE_STATE_TALKS;
}

void nsec_schedule_show_header(void) {
    gfx_setTextBackgroundColor(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts(nsec_schedule[schedule_index].menu_items[talk_selected].label);
    gfx_puts("\n");
    gfx_setTextBackgroundColor(SSD1306_BLACK, SSD1306_WHITE);
    gfx_puts("By: ");
    gfx_puts((char *) nsec_schedule[schedule_index].presenters[talk_selected]);
    gfx_puts("\n");
}

uint16_t nsec_schedule_get_header_length(void) {
    uint16_t len = strlen(nsec_schedule[schedule_index].menu_items[talk_selected].label) ;
    uint16_t header_length = len + (len % COLUMN_COUNT);
    len = strlen(nsec_schedule[schedule_index].presenters[talk_selected]) + 4;
    header_length += len + (len % COLUMN_COUNT);
    return header_length;
}

void _nsec_schedule_show_details(void) {
    gfx_fillRect(0, 8, 128, 56, SSD1306_BLACK);
    gfx_setCursor(0, 8);

    nsec_schedule_show_header();
    uint16_t header_length = nsec_schedule_get_header_length();

    // We calculate the number of character we can use for the description
    // we keep that value for scrolling
    char buffer[MAX_CHAR_UNDER_STATUS_BAR] = {0};
    description_index = MAX_CHAR_UNDER_STATUS_BAR - header_length;
    strncpy(buffer, nsec_schedule[schedule_index].descriptions[talk_selected], description_index);

    gfx_setTextBackgroundColor(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts(buffer);
    gfx_update();

    schedule_state = SCHEDULE_STATE_TALK_DETAILS;
}

void nsec_schedule_scroll_up_details(void) {
    //Is there enough place for the header ?
    uint16_t header_length = nsec_schedule_get_header_length();

    if (description_index < MAX_CHAR_UNDER_STATUS_BAR) {
        description_index = 0;
    } else {
        description_index -= MAX_CHAR_UNDER_STATUS_BAR;
    }

    if (header_length + description_index < MAX_CHAR_UNDER_STATUS_BAR) {
        _nsec_schedule_show_details();
        return;
    }

    gfx_fillRect(0, 8, 128, 56, SSD1306_BLACK);
    gfx_setCursor(0, 8);

    char buffer[MAX_CHAR_UNDER_STATUS_BAR] = {0};
    strncpy(buffer, nsec_schedule[schedule_index].descriptions[talk_selected] + description_index,
            MAX_CHAR_UNDER_STATUS_BAR);

    gfx_setTextBackgroundColor(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts(buffer);
    gfx_update();
}

void nsec_schedule_scroll_down_details(void) {
    if (description_index > strlen(nsec_schedule[schedule_index].descriptions[talk_selected])) {
        return;
    }

    gfx_fillRect(0, 8, 128, 56, SSD1306_BLACK);
    gfx_setCursor(0, 8);

    char buffer[MAX_CHAR_UNDER_STATUS_BAR] = {0};
    strncpy(buffer, nsec_schedule[schedule_index].descriptions[talk_selected] + description_index,
            MAX_CHAR_UNDER_STATUS_BAR);
    description_index += MAX_CHAR_UNDER_STATUS_BAR;

    gfx_setTextBackgroundColor(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts(buffer);
    gfx_update();
}

void nsec_schedule_show_details(uint8_t item) {
    talk_selected = item;
    menu_close();
    if(item < nsec_schedule[schedule_index].item_count) {
        _nsec_schedule_show_details();
    }
}

void nsec_schedule_show_tracks (uint8_t item) {
    menu_init(0, 8, 128, 56, ARRAY_SIZE(tracks_schedule_items), tracks_schedule_items);
    date_selected = item;
    schedule_state = SCHEDULE_STATE_TRACK;
}

void nsec_schedule_show_party(uint8_t item) {

}

void nsec_schedule_show_presenters(uint8_t item) {

}

static void nsec_schedule_button_handler(button_t button) {
    if (schedule_state == SCHEDULE_STATE_TALK_DETAILS && button == BUTTON_DOWN) {
        nsec_schedule_scroll_down_details();
    } else if (schedule_state == SCHEDULE_STATE_TALK_DETAILS && button == BUTTON_UP) {
        nsec_schedule_scroll_up_details();
    }

    if(button == BUTTON_BACK) {
        switch (schedule_state) {
            case SCHEDULE_STATE_TALK_DETAILS:
                nsec_schedule_show_talks(track_selected);
                break;
            case SCHEDULE_STATE_TALKS:
                nsec_schedule_show_tracks(date_selected);
                break;
            case SCHEDULE_STATE_TRACK:
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
