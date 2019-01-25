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
#include "drivers/ssd1306.h"
#include "drivers/controls.h"
#include "app_glue.h"
#include "gfx_effect.h"

#define ROW_COUNT                   6 // 8 - status bar
#define COLUMN_COUNT                21
#define MAX_CHAR_UNDER_STATUS_BAR   COLUMN_COUNT * ROW_COUNT

#define PRESENTER_COUNT             40

static void nsec_schedule_button_handler(button_t button);
void nsec_schedule_show_talks(uint8_t item);
void nsec_schedule_show_details(uint8_t item);
void nsec_schedule_show_tracks(uint8_t item);
void nsec_schedule_show_party(uint8_t item);
void nsec_schedule_show_presenters(uint8_t item);
void nsec_schedule_show_presenters_details(uint8_t item);

static menu_item_s days_schedule_items[] = {
    {
        .label = "Thursday, May 17th 2018",
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
        .label = "Track Ville-Marie",
        .handler = nsec_schedule_show_talks
    }, {
        .label = "Track De la Commune",
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

static menu_item_s presenters_items[PRESENTER_COUNT];

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

const char * presenters_all[] = {
    "Aaron Guzman",
    "Allan Wirth",
    "Charles F. Hamilton",
    "Daniel Bohannon",
    "Danny Cooper",
    "Dave Lewis",
    "Dimitry Snezhkov",
    "Eleanor Saitta",
    "Filip Kafka",
    "Israel Halle",
    "Jean-Marc Leblanc",
    "Jon Maurelian",
    "Karla Burnett",
    "Kristina Balaam",
    "Laurent Desaulniers",
    "Mahsa Alimardani",
    "Marc-Andre Labonte",
    "Mark El-Khoury",
    "Mark Mossberg",
    "Martijn Grooten",
    "Masarah Paquet-Clouston",
    "Melanie Segado",
    "Nicole Becher",
    "Olivier Arteau",
    "Olivier Bilodeau",
    "Paul Rascagneres",
    "Raphael Vinot",
    "Ron Bowes",
    "Robert Sell",
    "Saad Khadi",
    "Sarah Friend",
    "Silvia Valli",
    "Sebastien Larinier",
    "Stephane Graber",
    "Tanya Janca",
    "Thais Moreira Hamasaki",
    "Thomas Pornin",
    "Vikram Salunke",
    "Wan Mengyuan",
    "Yomna Nasser",
};

const char * presenters_detail[] = {
    "Aaron Guzman is a Security Consultant from the Los Angeles area with expertise in web app security, mobile app security, and embedded security. Mr. Guzman has spoken at several word-wide conferences which include: DEF CON, AppSec EU, AppSec USA, HackFest, Security Fest, HackMiami, 44Con, AusCERT as well as several regional BSides events. Furthermore, Aaron is a Chapter leader for the Open Web Application Security Project (OWASP) Los Angeles, Cloud Security Alliance SoCal (CSA SoCal), a Technical Editor, and author of \"IoT Penetration Testing Cookbook\" with Packt Publishing. He has contributed to many IoT security guidance publications from CSA, OWASP, Prpl, and several others. Aaron leads the OWASP Embedded Application Security project; providing practical guidance to address the most common firmware security bugs to the embedded and IoT community. Follow Aarons latest research on Twitter at @scriptingxss",
    "Allan Wirth and Danny Cooper are security researchers in the Adversarial Resilience group at Akamai, as well as administrators of BKPCTF.",
    "With more than 8 years of experience delivering Information Technology and Information Security services to various government and commercial clients such as a banks, nuclear industry and lay firms. Having the opportunity to perform RedTeam against complex and secured environment allowed him to develop a certain expertise that can be used to navigate through the target network without being detected. Since 2014 I'm also the proud owner of the RingZer0 Team website that have more than 28 000 members worldwide. The RingZer0 Team website is a hacking learning platform.",
    "Daniel Bohannon is a Senior Applied Security Researcher with FireEyes Advanced Practices Team with over seven years of operations, security and Incident Response experience. He is the author of Invoke-Obfuscation, Invoke-CradleCrafter, Invoke-DOSfuscation and co- author of the Revoke-Obfuscation detection framework. He has presented at numerous conferences including Black Hat USA, DEF CON, DerbyCon and BlueHat. Mr. Bohannon received a Master of Science in Information Security from the Georgia Institute of Technology and a Bachelor of Science in Computer Science from The University of Georgia.",
    "Allan Wirth and Danny Cooper are security researchers in the Adversarial Resilience group at Akamai, as well as administrators of BKPCTF.",
    "Dave has over two decades of industry experience. He has extensive experience in IT security operations and management. Currently, Dave is a Global Security Advocate for Akamai Technologies . He is the founder of the security site Liquidmatrix Security Digest and co-host of the Liquidmatrix podcast. Dave writes a column for Forbes and Huffington Post.",
    "Dimitry Snezhkov does not like to refer to himself in the third person :) but when he does he is a Sr. Security Consultant for X-Force Red at IBM, performing penetration testing, occasional Red Teaming and application security assessments.",
    "Eleanor Saitta is an independent security architecture and strategy consultant with media, finance, healthcare, infrastructure, and software clients across the US and Europe. She was previously the security architect for Etsy.com, and has worked for a number of commercial consultancies (Bishop Fox, IOACtive, and others) over the past fifteen years. Her work has encompassed everything from core security engineering and architecture work for Fortune 50 software firms to cross-domain security for news organizations and NGOs targeted by nation states. Her focus is on the ways task and experience design, system architecture, development process change, and operational changes can shift the balance of power between adversaries to bring better outcomes to users. Saitta is a co-founder and developer for Trike, an open source threat modeling methodology and tool which partially automates the art of security analysis and has contributed to the Briar and Mailpile secure messaging projects. She's on the advisory boards of the Freedom of the Press Foundation, the International Modern Media Institute, and the Calyx Institute, all organizations that look at freedom in the media and security online. Saitta is a regular speaker at industry conferences; past venues include O'Reilly Velocity, KiwiCon, ToorCon, CCC, Hack in The Box, and HOPE, among others. You can find her on twitter as @dymaxion, and at https://dymaxion.org",
    "Filip Kafka is a malware analyst at ESET's Malware Analysis Laboratory. His main responsibilities include detailed malware analyses and training new reverse engineers in the ESET Virus Lab, but his professional interests, as well as his latest research, focus on APTs. His experience as a speaker includes speaking at the Virus Bulletin conference, the AVAR conference, and at several events aimed at raising awareness about malware and computer security, presented for local universities. He also teaches a reverse engineering course at the Slovak University of Technology and the Comenius University and runs workshops of reverse engineering and malware research held in London, Brno, Bratislava.",
    "Israel Halle has a B.Eng. from the Ecole de Technologie Superieure (E.T.S.). He worked as a developer on the Merchant Protection and Checkout teams at Shopify. He also did malware analysis and reverse engineering contracting work for Google on their Safe browsing team. He is now working full-time developing the technology that powers Flare Systems. Israel has organized exploitation workshops at E.T.S. and at the NorthSec conference in addition to participating in multiples security CTFs, mostly working on binary reverse engineering and exploitation challenges.",
    "Currently working as a reverse engineer at EWA-Cadana, Jean-Marc has worked for multiple respected security enterprises for past 5 years including national security agencies and contract work at google. On top of his professional security research, he has done a lot of personal vulnerability research on large popular applications. He has successfully claim bug bounties from google chrome and shopify. He has presented multiple talk at various conferences including last years script engine hacking for fun and profit at northsec and why U A.F.ter calc? at Ihack Ottawa.",
    "Jon Maurelian is a security engineer at ConsenSys Diligence, where he works to ensure that Ethereum smart contracts are transparent, trustworthy, and reliable. He helped build a decentralized name registrar for the Ethereum Name Service; authoring the spec, and auditing the final implementation. He is a regular writer and speaker on smart contract security. Prior to joining ConsenSys, Maurelian worked at Coinbase.",
    "Karla has a varied offensive security background: she's reverse engineered train ticketing systems, written articles on TLS and SSH, and competed in the Defcon CTF finals for the last several years running. She officially works on authentication and application security at Stripe, but builds internal phishing campaigns when she has business hours to spare. She's cumulatively phished nearly half the company, has triggered many bouts of internal paranoia, and has built a reputation as being entirely untrustworthy when it comes to email.",
    "Kristina is an Application Security Engineer with Shopify. She builds web and mobile security tools and helps discover vulnerabilities in the existing platform. Prior to Shopify, Kristina led a team of developers, building web and mobile applications for enterprise and start-up clients. Kristina graduated with a Bachelor of Computer Science from McGill University in 2012. She volunteers with DEFCON Toronto, founded the Women in Tech Toronto book club, and is a novice boxer, book hoarder, and purveyor of fine cat gifs.",
    "Laurent is a team lead for a large security consulting firm, based in Montreal. He has conducted over 200 pentesting and red team engagements over the span of 10 years and is still enthusiatic about it. Laurent is also a challenge designer for Northsec and has given talks to CQSI, NCFTA, HackFest, RSI, Montrehack, Owasp Montreal and Northsec. Besides security, Laurent is interested in Lockpicking, magic and pickpocketting.",
    "Mahsa Alimardani has been doing research and work on the politics of Irans Internet for the past six years. She leads on some of Article 19's Iran digital rights projects while she does her DPhil at the Oxford Internet Institute at the University of Oxford, researching communications technology's and how they affect political participation in Iran's information control space. She has also been Iran editor for the citizen media platform Global Voices for the past five years.",
    "System administrator for more than 10 years, now doing penetration testing since 2016. I do have a strong interest in privacy and computer security. While not in front of a computer, I am either rock climbing or walking highlines.",
    "Mark El-Khoury is a Security Consultant with NCC Group, where he has been for over two years. Mark has been involved in a wide variety of security assessments, ranging from large web applications in various environments and frameworks, to native desktop applications and IoT devices. Mark also specializes in internal and external network infrastructure penetration testing, including IEEE 802.11 wireless assessments. Mark graduated from Syracuse University with a Master's degree in Computer Science, and has participated in many programming contests, including ACM ICPC, IEEE Xtreme, and Topcoder matches.",
    "Mark Mossberg is an engineer at Trail of Bits, and the lead developer for the Manticore project.",
    "Martijn Grooten is a lapsed mathematician who by chance ended up working in security - and loved it. He's spend more than a decade testing security software but his interest in security is broad and he has a weak spot for cryptography. He currently is Editor of Virus Bulletin.",
    "Masarah is a security researcher at GoSecure and one of Canadas decorated 150 scientific innovators. With her background in economics and criminology, she specializes in the study of market dynamics behind illicit online activities. Her primary goal is to conduct scientific research on online crime without falling into the alarmist side. She published in several peer-review journals, such as Global Crime, Social Networks and the International Journal for the Study of Drug Policy and presented at various international conferences including Black Hat Europe, Botconf and the American Society of Criminology.",
    "Melanie really likes brains and computers. This is why she co-founded NeuroTechX, a non-profit whose mission is to grow the global neurotechnology community. She is currently pursuing a PhD in cognitive neuroscience. Melanie spends her free time hacking on brain technology and thinking about its societal implications.",
    "Nicole Becher specializes in application security, red teaming, penetration testing, malware analysis and computer forenscics. OWASP Brooklyn Chapter Leader, OWASP DevSlop Project Leader, Adjunct Instructor at NYU, political junkie, marathoner, martial artist and animal lover. OWASP WASPY 2017 winner!",
    "Olivier Arteau is a security researcher that works for a large financial institution. In his early day, he was a web developer and transitioned into the security field during his university. He gave in the last few years a good amount of workshop for the usergroup MontreHack and is also part of the organization of a few CTF (Mini-CTF OWASP and NorthSec).",
    "Olivier Bilodeau is leading the Cybersecurity Research team at GoSecure. With more than 10 years of infosec experience, Olivier managed large networks and server farms, wrote open source network access control software and recently worked as a Malware Researcher. Passionate communicator, Olivier has spoken at several conferences like Defcon, Botconf, SecTor, Derbycon and many more. Invested in his community, he co-organizes MontreHack  a monthly workshop focused on applied information security through capture-the-flag challenges , he is in charge of NorthSecs training sessions and is hosting NorthSecs Hacker Jeopardy. His primary research interests include reverse-engineering tools, Linux and/or embedded malware and honeypots. To relax, he likes to participate in information security capture-the-flag competitions, work on various open-source projects and brew his own beer.",
    "Paul is a security researcher within Talos, Ciscos threat intelligence and research organization. As a researcher, he performs investigations to identify new threats and presents his findings as publications and at international security conferences throughout the world. He has been involved in security research for 7 years, mainly focusing on malware analysis, malware hunting and more specially on Advanced Persistence Threat campaigns and rootkit capabilities. He previously worked for several incident response team within the private and public sectors.",
    "Raphael Vinot is a longstanding member of Computer Incident Response Center Luxembourg (CIRCL) and of Malware Information Sharing Platform (MISP).",
    "Ron has worked in information security for more years than he can count, and has performed roles across the board. He currently works at Counter Hack, where he develops SANS Netwars, Holiday Hack Challenge, and other security games.",
    "Robert is a Senior IT Manager in the aerospace industry where he spends most of his time managing InfoSec teams. While his teams focus on the traditional blue/red team exercises, lately he has spent an increasing amount of time building defenses against social engineering. Robert has spoken about the rising SE risk at numerous events and on different security podcasts. In 2017 he competed at the Social Engineering Village Capture the Flag contest at Defcon 25. He placed third in this contest and since then has been teaching organizations how to defend against SE attacks and reduce the OSINT footprint. Robert is also a nine year veteran with Search & Rescue in British Columbia, Canada. In his SAR capacity, Robert is a Team Leader, Trainer, Marine Rescue Technician, Swift Water Technician and Tracker. While one may think that SAR has little do to with InfoSec, tracking lost subjects in the back country has many of the same qualities as tracking individuals or organizations online with OSINT.",
    "Saad Kadhi, head of CERT Banque de France and TheHive Project leader, has over 18 years of experience in cybersecurity. He discovered incident response and digital forensics in early 2008 and has been working exclusively in this fascinating field since then. He built a CSIRT at a French multinational food-products corporation and worked as an analyst at CERT Societe Generale before joining the French national central bank where he leads a team of 20 analysts. He frequently writes information security articles in a leading French magazine. He also co-organizes the Botconf security conference.",
    "Sarah Friend is a software engineer working at a large blockchain development studio on tools for financial transparency/accounting. When not doing that, she creates games and other interactive experiences. She has recently exhibited with Furtherfield at the Neon Festival in Dundee, Scotland, and presented at the Montreal International Games Showcase. In 2018, she will be part of the MoneyLab program by the Institute for Networked Culture in London, UK and at Transmediale in Berlin.",
    "Security researcher from Estonia, working as a web-application pentester in Clarified Security.",
    "Sebastien Larinier is a freelance Senior Researcher and Incident Handler who created the CERT Sekoia located in Paris. Member of the honeyproject chapter France and co-organizer of botconf. Sebastien focuses his work on botnet hunting, malware analysis, network forensics, early compromise detection, forensic and incident response. As a Python addict, he supports different opensource projects like FastIR, veri-sig, Oletools, pymisp, and malcom...",
    "Stephane Graber works as the technical lead for LXD at Canonical Ltd. He is the upstream project leader for LXC and LXD and a frequent speaker and track leader at various containers and other Linux related events. Stephane is also a long time contributor to the Ubuntu Linux distribution as an Ubuntu Core Developer and he currently sits on the Ubuntu Technical Board. On his spare time, Stephane helps organize a yearly security conference and contest in Montreal, Northsec, where his knowledge of Linux and network infrastructure is used to simulate the most complex of environments for the contestants.",
    "Tanya Janca is a senior cloud advocate for Microsoft, specializing in application security; evangelizing software security and advocating for developers through public speaking, her open source project OWASP DevSlop, and various forms of teaching via workshops, blogs and community events. As an ethical hacker, OWASP Project and Chapter Leader, software developer, effective altruist and professional computer geek of 20+ years, she is a person who is truly fascinated by the science of computer science.",
    "Thais Moreira Hamasaki is a malware researcher, who focus on static analysis, reverse engineering and logical programming. Thais started her career within the anti-virus industry working on data and malware analysis, where she developed her knowledge on threat protection systems. She won the best rookie speaker award from BSides London for her first talk about Using SMT solvers to deobfuscate malware binaries. Recent research topics include malware binary code deobfuscation, generic unpacking and malware analysis automation. She is an active member of the Dusseldorf Hackerspace, where she also leads the groups for Reverse Engineering and x86 Assembly. In her free time, you can find Thais building tools, cooking or climbing somewhere offline.",
    "Thomas Pornin is a cryptographer and researcher, doing cryptography consulting at NCC Group. He is the author of BearSSL, a secure and compact SSL/TLS library.",
    "Vikram is the founder of Vmaskers, and a professional pentester. He has led 100+ pentests over the past years, compromising highly sensitive and secured enterprise networks. His primary responsibilities in his recent job roles were to look after enterprise network security, manage security automation and build internal tools to fight security attacks. He has also discovered serious security flaws in many unique product giants all over the world. He has worked in various domains including Pentesting, Reverse Engineering, Fuzzing, Exploitation ,Source Code Auditing and Mobile application security research. He helps the community by uploading regular InfoSec videos on youtube (https://www.youtube.com/VikramSalunke). He has also previously spoken and trained at numerous security conferences all around the world including CHCon, OWASP AppSec Africa, CrikeyCon, CanSecWest, OWASP New Zealand Day, NolaCon, LayerOne, ShakaCon, OWASP AppSec California and will be training in Hack in The Box (HITB), InfoSec in the City, BlackHat USA 2018 etc.",
    "Wan is a security engineer in DeNA Co., Ltd. He gives pen testing and security consulting for in-house developers. His main focus is on pen testing, game security and reverse engineering. He loves writing tools for game hacking / analyzing and publishing them on Github.",
    "Yomna likes mathematical cryptography, geometry, and clear, succinct explanations.",
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
    "Robert discusses his third place experience at the Defcon 2017 SE CTF and how his efforts clearly show how easy it is to get sensitive information from any organization. The 2017 Verizon report clearly shows the dramatic growth rate of social engineering attacks and Robert demonstrates how he collected hundreds of data points from the target organization using OSINT techniques. He then goes into the vishing strategy he implemented to maximize the points he collected in the 20 minute live contest. Without much effort Robert was able to know their VPN, OS, patch level, executive personal cell phone numbers and place of residence. Robert lifts the curtain of the social engineering world by showing tricks of the trade such as the incorrect confirmation which is one of many methods to loosen the tongues of his marks. Robert then shows the pretexts he designed to attack companies and the emotional response each pretext is designed to trigger. By knowing these patters we can better educate our staff. With that much information at his fingertips, how long would it take him to convince your executive to make a bank transfer? If your organization lost a few million dollars due to social engineering, who would be to blame? Are you insured for that? Who is getting fired? Robert wraps up his talk with a series of strategies companies can take to reduce exposure and risk. He goes over current exposure, building defenses, getting on the offense and finally... a culture shift",
    NULL,
    "Ethereum is a novel, decentralized computation platform that has quickly risen in popularity since it was introduced in 2014, and currently controls the equivalent of one hundred ten billion dollars. At its foundation is a virtual machine which executes smart contracts: programs that ultimately control the majority of the value transfer within the network. As with most other types of programs, correctness is very important for smart contracts. However, somewhat uniquely to Ethereum, incorrectness can have a direct financial cost, as evidenced by a variety of high profile attacks involving the loss of hundreds of millions of dollars. The error-prone nature of developing smart contracts and the increasing amounts of capital processed by them motivates the development of analysis tools to assist in automated error and vulnerability discovery. In this talk, we describe our work towards smart contract analysis tooling for Ethereum, which focuses on a modern technique called symbolic execution. We provide context around both Ethereum and symbolic execution, and then discuss the unique technical challenges involved with combining the two, touching on topics including blockchains, constraint solvers, and virtual machine internals. Lastly, we present Manticore: an open source symbolic execution tool which we have used to enhance smart contract security audits.",
    NULL,
    "Often defenders worry about the intangible security problems. Defenders need to concentrate their efforts defending the enterprise by focusing on the fundamentals. Too often issues such as patching or system configuration failures lead to system compromise. These along with issues such as SQL injection are preventable problems. Defenders can best protect their digital assets by first understanding the sheer magnitude that a data breach can have on an enterprise. In this talk I review my findings after analyzing hundreds of data breach disclosures as it pertains to what went wrong.",
    NULL,
};

const char * descriptions_thursday_track_2[] = {
    "Elliptic curves are relatively obscure mathematical objects: you can get a PhD in maths without ever having come across them. Yet these objects play an important role in modern cryptography and as such are found in most HTTPS connections, in Bitcoin, and in a large number of other places. To really understand elliptic curve cryptography (ECC) to the point that you can implement algorithms, you'd have to study the maths behind it. This talk assumes that you haven't studied the maths, but just want to understand what ECC is about, how is works and how it is implemented. It will discuss how 'point addition' works and how the Elliptic Curve Diffie-Hellman algorithm is used, for example in HTTPS - and how you can find it using Wireshark. It will explain how to use elliptic curve for digital signatures and why you don't want to be like Sony when it comes to implementing them. It will discuss how ECC was used in an infamous random number generator and, finally, will take a brief look at the use of elliptic curves in post-quantum algorithms. The goal of this talk is to keep things simple and understandable and no knowledge of maths is assumed. The talk won't make you an expert on ECC -- that would take years of studying. But it might help you understand the context a bit better when you come across them in your research. And hopefully it will also be a little bit fun.",
    NULL,
    "Hacking Team came into the spotlight of the security industry following its damaging data breach in July 2015. The leaked data revealed several 0-day exploits being used and sold to governments, and confirmed Hacking Teams suspected business with oppressive regimes. But what happened to Hacking Team after one of the most famous hacks of recent years? Hacking Teams flagship product, the Remote Control System (RCS), was detected in the wild in the beginning of 2018 in fourteen countries, including those contributing to previous criticism of the companys practices. We will present the evidence that convinced us that the new post-hack Hacking Team samples can be traced back to a single group  not just any group  but Hacking Teams developers themselves. Furthermore, we intend to share previously undisclosed insights into Hacking Teams post-leak operations, including the targeting of diplomats in Africa, uncover digital certificates used to sign the malware, and share details of the distribution vectors used to target the victims. We will compare the functionality of the post-leak samples to that in the leaked source code. To help other security researchers well provide tips on how to efficiently extract details from these newer VMProtect-packed RCS samples. Finally, we will show how Hacking Team sets up companies and purchases certificates for them.",
    NULL,
    "Stupid tricks for everyone! This talk will present very simple, low tech attacks to better achieve your goals, both attack and defense. From a defense standpoint, this talk will present simple tricks to identify Responder on your network, pinpoint BurpSuite activities, block some active crimeware and other simple tricks. Offensive tricks include a very simple NAC bypass, even more physical pentesting tricks and some very simple changes to social engineering that can help a lot.",
    NULL,
    "Prototype pollution is a term that was coined many years ago in the JavaScript community to designate libraries that added extension methods to the prototype of base objects like \"Object\", \"String\" or \"Function\". This was very rapidly considered a bad practice as it introduced unexpected behavior in applications. In this presentation, we will analyze the problem of prototype pollution from a different angle. What if an attacker could pollute the prototype of the base object with his own value? What APIs allow such pollution? What can be done with it?",
    NULL,
    "At a certain level of technological sophistication you gain the ability to grant others read, write, and execute permissions to your brain. That ability creates a unique set of security concerns. In this talk we will discuss the current state of both brain scanning and brain stimulation technology, the practical implications of merging brains with artificial intelligence, and the role infosec can play in shaping the dystopic cyberpunk future that were currently careening towards.",
    NULL,
    "PowerShell gave us a super-highway of convenient building blocks for offensive toolkits and operational automation. In the post offensive PowerShell world, a move in the direction of .NET implants may be a desirable option in some cases. However, Red Teams are faced with challenges when moving automation down into managed code. Can .NET based toolkits maintain flexibility, quick in-field retooling and operational security in the face of current detection mechanisms? We think the answer is yes. In this talk, we will focus on quick in-field retooling and dynamic execution aspect of .NET implants as the crucial trait to overcome static defensive mechanisms. We will dive deeper into OpSec lessons learned from dynamic code compilation. We will attempt to move beyond static nature of .NET assemblies, into reflective .NET DLR. We will showcase on-the-fly access to native Windows API and discuss methods of hiding sensitive aspects of execution in the managed code memory. All that, with the help of the DLRium Managed Execution toolkit we have in development.",
    NULL,
    "In typical enterprise networks today, ingress filtering is taken care of by firewall or similar devices. Unfortunately, the ability of devices and applications to reach the outside world is often overlooked or intentionnaly left open as Web services might need to be reacheable. We will present a fork of an OpenSSH daemon, that is able to exploit the often loose egress filtering and maneuver around network restrictions. Designed for more comfortable post-exploitation, it also extends regular forwarding and tunneling abilities in order to circumvent network rules that may otherwise hinder lateral movement. In addition, it can also act as a regular SSH server listening for an incoming connection, and provides reliable interactive shell access (must be able to run top, sudo, screen, vi, etc) as opposed to crafted reverse shells or even meterpreter which allow basic commands but fail at interactive ones.",
    "Every day, most people who uses a computer will either run applications on untrusted networks (like public wifi) or run application that will run untrusted scripts on their machine. Whether it is a browser running javascript, a cryptocurrencys smart contracts or even a script from a map or game mod, scripting engines like these tend to have large attack surface for vulnerabilities and they are usually quite exploitable especially when they are use after free bugs. How ever, finding these bug in large open source projects can be a bit intimidating. In this talk I will present various tools that I used for finding vulnerabilities in open source software. I will try to demonstrate the various bug patterns and how I look for them using examples in everyday software. I will explain how to go from a bug to a vulnerable bug. Finally, I will explain what is use-after-free (auf) and the bug patterns to look for. Use uaf bugs can be quite tricky to find and quite complicated to exploit. But can be quite dangerous if an attacker understands them well. To demonstrate how powerful uaf in a scripting engine can be, I will walk the audience through a uaf in a modern browser bugs and the some techniques used to exploit them.",
};

const char * descriptions_thursday_workshops[] = {
    NULL,
    "This workshop will teach you how to attack applications secured by Firewalls, IDS/IPS, Antivirus, WAF. The presenter will describe the newest bypassing techniques and provide a systematic and practical approach on how to bypass modern access control mechanisms. This workshop contains lot of demos. Everyone is now using Firewalls, IDS/IPS, Load Balancers with multiple features such as algorithms, signatures etc. Since the beginning, filter obfuscation and evading technique have been there. These mechanisms provide multiple layers of defense, so bypassing them is an important aspect of pentesting. This workshop describes different techniques to bypass these mechanisms. We will see them in action with multiple demos. Just bring your laptop to learn these attacks practically.",
    NULL,
    "Fuzzing could be summed up as a testing method feeding random inputs to a program. Where a more traditional approach to testing relies on manual design of tests based on known assumptions, fuzzing bring an automated mean of creating test cases. Although a single test generated by a fuzzer is unlikely to find any defaults, millions of them in quick iterations makes it very likely to trigger unexpected behaviours and crashes. With the rise of smarter fuzzers, fuzzing has become an efficient and reliable way to test most edge cases of a program and makes it possible to cover very large programs that would require otherwise a large amount of effort from manual reviewing and testing. The low amount of manual intervention required to setup a modern smart fuzzer dismiss any pretexts a developer or security research might have to not fuzz its project. If you aren't fuzzing, the bad guys will (and find all the bugs that comes with it). This workshop aims to introduce the basic concepts of fuzzing to the participants and to enable them to make fuzzing a critical step of their testing process. The class is going to start with a quick introduction about the concepts of fuzzing, why they should do it and some benefits other organisations have gain from it. The workshop will then move on to an hands-on approach on how to set up AFL and run it against a program and how to interpret the outputs. Most of the exercice will turn around a sample program with intentional bugs and gotchas, and once the participants will have an understanding of the basis, they will be walked through real world scenarios. Finally, a time will be allocated at the end for the participants to fuzz a project of their choice with the assistance of the presenters.",
    NULL,
    "The objective of this workshop is to dive into Capture-The-Flag (CTF) competitions. First, by introducing them. Then by helping both individuals and teams prepare but also evolve in their practice of applied cybersecurity. We will have various levels (easy, medium, hard) of CTF challenges in several categories (binaries, exploitation, Web, forensics) and we will give hints and solutions during the workshop. This is meant to be for CTF first timers. Seasoned players should play NorthSecs official CTF instead.",
    NULL,
    "The goal of the tutorial is to familiarize participants with Incident Response and Cyber Threat Intelligence using TheHive  a Security Incident Response Platform, Cortex  a powerful observable analysis engine, and MISP  the de facto standard platform for threat sharing.",
};

const char * descriptions_friday_track_1[] = {
    "Ron built his security career in a unique way: writing cheats for video games. In highschool, while others were having fun, he was trying to find new and creative ways to confuse Starcraft. In this presentation, he will look at some of the major hacks, cheats, and glitches in video games, from famous ones (like arbitrary code execution in Super Mario World) to obscure ones (like stacking buildings in Starcraft). But more importantly, he will tie these into modern vulnerabilities: the Legend of Zelda \"bottle glitch\" is a type-confusion vulnerability, for example: similar vulnerabilities in normal software could lead to remote code execution. This talk will bridge video game cheating with real-world security vulnerabilities, and explore the history of both!",
    NULL,
    "Over the decades, various security techniques to mitigate desktop specific vulnerabilities have been developed which makes it difficult to successfully exploit traditional desktop applications. With the rise of Electron framework, it became possible to develop multi-platform desktop applications by using commonly known web technologies. Developed by the Github team, Electron has already become amazingly popular (used by Skype, Slack, Wire, Wordpress and so many other big names), bringing adventurous web app developers to explore the desktop environment. These same developers who make the XSS to be the most common web vulnerability are now bringing the same mistakes to a whole new environment. While XSS in the web applications is bounded by the browser, the same does not apply to Electron applications. Making the same kind of mistakes in an Electron application widens the attack surface of desktop applications, where XSS can end up being so much more dangerous. So in this talk, I will discuss the Electron framework and the related security issues, its wonderful features getting me a bunch of CVEs, possible attack vectors and the developers in the dark about these issues. AND as Electron apps do not like to play in the sandbox, this talk will DEMO Electron applications found to be vulnerable, gaining code execution from XSS.",
    NULL,
    "Smart contract security is a brave, new, and sometimes terrible field. This presentation will take you through some of the most famous vulnerabilities of these first few years (from the Dao hack, to the Parity wallet vulnerabilities ... and including less-well-known but very interesting events like the DDOS attacks from late 2016). We'll explain the details of how these attacks work(ed), some of the idiosyncrasies of Ethereum, and through these examples some general principles of smart contract security.",
    NULL,
    "How do Iranians experience the Internet? Various hurdles and risks exist for Iranians and including outside actors like American technology companies. This talk will assess the state of the Internet in Iran, discuss things like the threats of hacking from the Iranian cyber army; how the government are arresting Iranians for their online activities; the most recent policies and laws for censorship, surveillance and encryption; and the policies and relationships of foreign technology companies like Apple, Twitter and Telegram with Iran, and the ways they are affecting the everyday lives of Iranians. This talk will effectively map out how the Internet continues to be a tight and controlled space in Iran, and what efforts are being done and can be done to make the Iranian Internet a more accessible and secure space.",
    NULL,
    "Skilled attackers continually seek out new attack vectors and effective ways of obfuscating old techniques to evade detection. Active defenders can attest to attackers prolific obfuscation of JavaScript, VBScript and PowerShell payloads given the ample availability of obfuscation frameworks and their effectiveness at evading many of todays defenses. However, advanced defenders are increasingly detecting this obfuscation with help from the data science community. This approach paired with deeper visibility into memory-resident payloads via interfaces like Microsofts Antimalware Scan Interface (AMSI) is causing some Red Teamers to shift tradecraft to languages that offer defenders less visibility. But what are attackers using in the wild? In the past year numerous APT and FIN (Financial) threat actors have increasingly introduced obfuscation techniques into their usage of native Windows binaries like wscript.exe, regsvr32.exe and cmd.exe. Some simple approaches entail randomly adding cmd.exes caret (^) escape character to command arguments. More interesting techniques like those employed by APT32, FIN7 and FIN8 involve quotes, parentheses and standard input.",
    NULL,
    "DNS Rebinding attacks have re-entered the spotlight, largely owing to recent high-profile disclosures by Tavis Ormandy including RCE in the Blizzard Update Agent triggered from the browser. However, given the vast amount of consumer software in circulation today and the apparent frequency with which the design (anti)pattern of treating localhost as secure occurs, it is likely that many vulnerable services still exist. In this talk, we will present a set of tools we created to make performing DNS Rebinding attacks fast and easy at scale, discuss how these tools can be used to perform network reconnaissance from inside a browser, and present an opt-in localhost census page that uses DNS rebinding to enumerate localhost services listening for HTTP on the visitors computer, and adds the results to a database.",
};

const char * descriptions_friday_track_2[] = {
    "Machine learning can be useful for helping analysts and reverse engineers. This presentation will explain how to transform data to use machine-learning algorithms to categorize a malware zoo. To cluster a set of (numerical) objects is to group them into meaningful categories. We want objects in the same group to be closer (or more similar) to each other than to those in other groups. Such groups of similar objects are called clusters. When data is labeled, this problem is called supervised clustering. It is a difficult problem but easier than the unsupervised clustering problem we have when data is not labeled. All our experiments have been done with code written in Python and we have mainly used scikit-learn. With the dataset the Zoo, we present how to use unsupervised algorithms on labeled datasets to validate the model. When the model is finalized, the resulting clusters can be used to automatically generate yara rules in order to hunt down the malware.",
    NULL,
    "Commercial Android emulators such as NOX, BlueStacks and Leidian are very popular at the moment and most games can run on these emulators fast and soundly. The bad news for game vendors is that these emulators are usually shipped with root permission in the first place. On the other hand, cheating tools developers are happy because they can easily distribute their tools to abusers without requiring the abusers to have a physical rooted device, nor do they need to perform trivial tuning for different Android OS / firmware version. However, luckily for game vendors, commercial Android emulators usually use an x86/ARM mixed-mode emulation for speed-up. As a result, a standard native hooking/DBI framework won't work on this kind of platform. This drawback could discourage the cheating developers. In this talk, I will introduce a native hooking framework on such a kind of mixed-mode emulators. The talk will include the process start routine of both command-line applications and Android JNI applications as well as how these routines differ on an emulator. The different emulation strategies adopted by different emulators and runtime environments (Dalvik/ART) will also be discussed. Based on these knowledge, I will explain why the existing hooking/DBI frameworks do not work on these emulators and how to make one that works. Lastly, I will present a demo of using this hooking framework to cheat a game on emulator. With this demo, I will discuss how the dark market of mobile game cheating may develop in the foreseeable future.",
    NULL,
    "The first commercially-available Android device was released in 2008. Despite its nearly 10-year public lifespan, the OS still poses numerous security challenges. Now, as mobile becomes an increasingly popular platform for consumers, we're faced with the challenge of protecting these consumers from new, quickly evolving threats. Well discuss why Android security is so much more challenging for software developers compared to iOS and the web, look at the most common attack vectors for the operating system, and walk through best practices for guarding against them.",
    NULL,
    "Supply chain attacks are often long thought about and often overlooked in terms of how well a business prepares itself for any associated compromise or breach. 2017 has truly marked itself as 'The Year Of The Supply Chain Attack' and marked a turning point concerning supply chain attacks. Talos was involved in two major campaigns: MeDoc compromise that paralyzed the Ukraine and CCleaner compromise that impacted a reported 2.27M consumers. In this presentation we will first present these two cases. In both cases, we will present how the attackers modified a legitimate application and what was the result of the modification. We will explain the purpose of the attackers and the malware used against the victims. For the MeDoc compromise, we were directly involved in the incident response and we will provide a timeline of the events to give an idea of the before, during and after picture associated with Nyetya and MeDoc. Concerning the CCleaner compromise, we will provide some data and statistics from the attacker's database and the profiles of the targeted organizations. In a second part, we will speak globally about supply chain attacks. We will remember that it's not the first time in the history that this kind of attacks occurred and we will finally open the discussion on the future of this attacks.",
    NULL,
    "IMSI-catchers, also known as cell-site simulators, are devices that let their operators track cell users, interfere with their calls/texts, and mount other privacy-invasive attacks. While research around IMSI-catchers has been gaining traction over the years, there hasnt been much effort into making the more technical results accessible outside of academia and niche hardware hacking circles. The goal of this talk is to remedy that. This talk will be a deep technical dive into how cell networks interact with user equipment, the details of how IMSI-catchers exploit their design flaws, what goes into building an IMSI-catcher (hypothetically, of course!), the relationship Canadian & American law enforcement have with these devices, and steps one can take to protect themselves.",
    NULL,
    "This talk will describe many issues that a redteamer may face during a Red Team exercise. Being stealth is one of them; avoiding detection of your lateral movements, phishing campaign and post exploitation are crucial to succeed. Over the years I've developed tools and different approaches that can be used during standard engagement and Red Team to remain stealth and move more efficiently into your victim network. During the presentation several techniques will be described and analyzed to understand the idea behind them.",
};

const char * descriptions_friday_workshops[] = {
    NULL,
    "This workshop will briefly go over the Wi-Fi basics and known security issues, covering WPA2-Personal, WPA2-Enterprise, WPS, and then focusing on the most recent developments in Wi-Fi such as KRACK, and will include hands-on labs. The workshop will also cover direct attacks against wireless clients and access points, such as router vulnerabilities, rogue access points and denial-of-service attacks.",
    NULL,
    "Fully understanding a botnet often requires a researcher to go beyond standard reverse-engineering practices and explore the malwares network traffic. The latter can provide meaningful information on the evolution of a malwares activity. However, it is often disregarded in malware research due to time constraints and publication pressures. The workshop is about overcoming such constraints by providing a powerful workflow to conduct quick analysis of malicious traffic. The data science approach presented capitalizes on open-source tools (Wireshark/Tshark, Bash with GNU parallel) and valuable python libraries (ipython, mitmproxy, pandas, matplotlib). During the workshop, participants will do practical technical labs with datasets from our recent botnet investigation. They will learn how to quickly find patterns, plot graphs and interpret data in a meaningful way. Although the exercises will focus on botnets data, the tools and skills learned will be useful to all sorts of context. Moreover, to ensure that participants take the most out of the workshop, it will be built in a way to allow them to easily replicate the data-analysis environment at home and reproduce similar analysis with their own traffic data.",
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
    SCHEDULE_STATE_PRESENTERS,
    SCHEDULE_STATE_PRESENTERS_DETAILS,
};

static enum schedule_state schedule_state = SCHEDULE_STATE_CLOSED;
static uint16_t description_index = 0;
static uint8_t date_selected = 0;
static uint8_t track_selected = 0;
static uint8_t talk_selected = 0;
static uint8_t presenter_selected = 0;
static uint8_t schedule_index = 0; // Keep track of our index in the scheduler array

void nsec_schedule_show_dates(void) {
    menu_init(0, 12, 128, 56, ARRAY_SIZE(days_schedule_items), days_schedule_items);
    schedule_state = SCHEDULE_STATE_DATES;
    nsec_controls_add_handler(nsec_schedule_button_handler);
}

void nsec_schedule_return_to_talks(void) {
    description_index = 0;
    schedule_state = SCHEDULE_STATE_TALKS;
    menu_open();
    menu_ui_redraw_all();
}

void nsec_schedule_show_talks(uint8_t item) {
    track_selected = item;
    schedule_index = (date_selected * 3) + track_selected;
    menu_init(0, 12, 128, 56, nsec_schedule[schedule_index].item_count, nsec_schedule[schedule_index].menu_items);
    schedule_state = SCHEDULE_STATE_TALKS;
}

void nsec_schedule_show_header(void) {
    gfx_set_text_background_color(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts(nsec_schedule[schedule_index].menu_items[talk_selected].label);
    gfx_puts("\n");
    gfx_set_text_background_color(SSD1306_BLACK, SSD1306_WHITE);
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
    gfx_fill_rect(0, 8, 128, 56, SSD1306_BLACK);
    gfx_set_cursor(0, 16);

    nsec_schedule_show_header();
    uint16_t header_length = nsec_schedule_get_header_length();

    // We calculate the number of character we can use for the description
    // we keep that value for scrolling
    char buffer[MAX_CHAR_UNDER_STATUS_BAR] = {0};
    if(header_length >= MAX_CHAR_UNDER_STATUS_BAR) {
        header_length = MAX_CHAR_UNDER_STATUS_BAR - 1;
    }
    description_index = MAX_CHAR_UNDER_STATUS_BAR - header_length;
    strncpy(buffer, nsec_schedule[schedule_index].descriptions[talk_selected], description_index);

    gfx_set_text_background_color(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts(buffer);
    gfx_update();

    schedule_state = SCHEDULE_STATE_TALK_DETAILS;
}

void nsec_schedule_scroll_up_details(bool change_direction) {
    //Is there enough place for the header ?
    uint16_t header_length = nsec_schedule_get_header_length();

    if (description_index < MAX_CHAR_UNDER_STATUS_BAR) {
        description_index = 0;
    } else {
        description_index -= MAX_CHAR_UNDER_STATUS_BAR;
        if (change_direction) {
            description_index -= MAX_CHAR_UNDER_STATUS_BAR;
        }
    }

    if (header_length + description_index < MAX_CHAR_UNDER_STATUS_BAR) {
        _nsec_schedule_show_details();
        return;
    }

    gfx_fill_rect(0, 8, 128, 56, SSD1306_BLACK);
    gfx_set_cursor(0, 12);

    char buffer[MAX_CHAR_UNDER_STATUS_BAR] = {0};
    strncpy(buffer, nsec_schedule[schedule_index].descriptions[talk_selected] + description_index,
            MAX_CHAR_UNDER_STATUS_BAR);

    gfx_set_text_background_color(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts(buffer);
    gfx_update();
}

void nsec_schedule_scroll_down_details(bool change_direction) {
    if (description_index > strlen(nsec_schedule[schedule_index].descriptions[talk_selected])) {
        return;
    }

    gfx_fill_rect(0, 8, 128, 56, SSD1306_BLACK);
    gfx_set_cursor(0, 16);

    if (change_direction) {
        description_index += MAX_CHAR_UNDER_STATUS_BAR;
    }

    char buffer[MAX_CHAR_UNDER_STATUS_BAR] = {0};
    strncpy(buffer, nsec_schedule[schedule_index].descriptions[talk_selected] + description_index,
            MAX_CHAR_UNDER_STATUS_BAR);
    description_index += MAX_CHAR_UNDER_STATUS_BAR;

    gfx_set_text_background_color(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts(buffer);
    gfx_update();
}

void nsec_schedule_scroll_up_presenters_details(bool change_direction) {
    if (description_index < MAX_CHAR_UNDER_STATUS_BAR) {
        description_index = 0;
    } else {
        description_index -= MAX_CHAR_UNDER_STATUS_BAR;
        if (change_direction) {
            description_index -= MAX_CHAR_UNDER_STATUS_BAR;
        }
    }

    gfx_fill_rect(0, 8, 128, 56, SSD1306_BLACK);
    gfx_set_cursor(0, 16);

    char buffer[MAX_CHAR_UNDER_STATUS_BAR] = {0};
    strncpy(buffer, presenters_detail[presenter_selected] + description_index,
            MAX_CHAR_UNDER_STATUS_BAR);

    gfx_set_text_background_color(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts(buffer);
    gfx_update();
}

void nsec_schedule_scroll_down_presenters_details(bool change_direction) {
    if (description_index > strlen(presenters_detail[presenter_selected])) {
        return;
    }

    gfx_fill_rect(0, 8, 128, 56, SSD1306_BLACK);
    gfx_set_cursor(0, 16);

    if (change_direction) {
        description_index += MAX_CHAR_UNDER_STATUS_BAR;
    }

    char buffer[MAX_CHAR_UNDER_STATUS_BAR] = {0};
    strncpy(buffer, presenters_detail[presenter_selected] + description_index,
            MAX_CHAR_UNDER_STATUS_BAR);
    description_index += MAX_CHAR_UNDER_STATUS_BAR;

    gfx_set_text_background_color(SSD1306_WHITE, SSD1306_BLACK);
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
    menu_init(0, 12, 128, 56, ARRAY_SIZE(tracks_schedule_items), tracks_schedule_items);
    date_selected = item;
    schedule_state = SCHEDULE_STATE_TRACK;
}

void nsec_schedule_show_party(uint8_t item) {
}

void nsec_schedule_show_presenters_details(uint8_t item) {
    schedule_state = SCHEDULE_STATE_PRESENTERS_DETAILS;
    presenter_selected = item;
    menu_close();
    gfx_fill_rect(0, 8, 128, 56, SSD1306_BLACK);
    gfx_set_cursor(0, 16);
    gfx_set_text_background_color(SSD1306_WHITE, SSD1306_BLACK);
    gfx_puts((char *) presenters_detail[presenter_selected]);
    description_index = MAX_CHAR_UNDER_STATUS_BAR;
    gfx_update();
}

void nsec_schedule_return_to_presenters(void) {
    description_index = 0;
    schedule_state = SCHEDULE_STATE_PRESENTERS;
    menu_open();
    menu_ui_redraw_all();
}

void nsec_schedule_show_presenters(uint8_t item) {
    for (int i=0; i<PRESENTER_COUNT; i++) {
        presenters_items[i].label = presenters_all[i];
        presenters_items[i].handler = nsec_schedule_show_presenters_details;
    }
    menu_init(0, 12, 128, 56, ARRAY_SIZE(presenters_items), presenters_items);
    description_index = 0;
    schedule_state = SCHEDULE_STATE_PRESENTERS;
}

static void nsec_schedule_button_handler(button_t button) {
    static button_t last_pressed_button = BUTTON_ENTER;
    if (schedule_state == SCHEDULE_STATE_TALK_DETAILS && button == BUTTON_DOWN) {
        if (last_pressed_button == BUTTON_UP) {
            nsec_schedule_scroll_down_details(true);
        } else {
            nsec_schedule_scroll_down_details(false);
        }
        last_pressed_button = BUTTON_DOWN;
    } else if (schedule_state == SCHEDULE_STATE_TALK_DETAILS && button == BUTTON_UP) {
        if (last_pressed_button == BUTTON_DOWN) {
            nsec_schedule_scroll_up_details(true);
        } else {
            nsec_schedule_scroll_up_details(false);
        }
        last_pressed_button = BUTTON_UP;
    }

    if (schedule_state == SCHEDULE_STATE_PRESENTERS_DETAILS && button == BUTTON_DOWN) {
        if (last_pressed_button == BUTTON_UP) {
            nsec_schedule_scroll_down_presenters_details(true);
        } else {
            nsec_schedule_scroll_down_presenters_details(false);
        }
        last_pressed_button = BUTTON_DOWN;
    } else if (schedule_state == SCHEDULE_STATE_PRESENTERS_DETAILS && button == BUTTON_UP) {
        if (last_pressed_button == BUTTON_DOWN) {
            nsec_schedule_scroll_up_presenters_details(true);
        } else {
            nsec_schedule_scroll_up_presenters_details(false);
        }
        last_pressed_button = BUTTON_UP;
    }

    if(button == BUTTON_BACK) {
        switch (schedule_state) {
            case SCHEDULE_STATE_PRESENTERS_DETAILS:
                last_pressed_button = BUTTON_ENTER;
                nsec_schedule_return_to_presenters();
                break;
            case SCHEDULE_STATE_TALK_DETAILS:
                last_pressed_button = BUTTON_ENTER;
                nsec_schedule_return_to_talks();
                break;
            case SCHEDULE_STATE_TALKS:
                nsec_schedule_show_tracks(date_selected);
                break;
            case SCHEDULE_STATE_TRACK:
            case SCHEDULE_STATE_PRESENTERS:
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
