#!/usr/bin/env python3

#
# Copyright 2019 Simon Marchi <simon.marchi@polymtl.ca>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

# Process html files downloaded with download-conf-html.py.
#
# Use as:
#
#     python3 gen-conf-structs.py > ../src/app/conf_sched.inc

import bs4
import sys
import re
import glob

loc2track = {
    'Conf 1': 0,
    'Conf 2': 1,
    'Workshop 1 (Big)': 2,
    'Workshop 2 (Salon du president)': 3,
    'Workshop 3 (classroom)': 4,
    'Workshop 4 (classroom)': 5,
}

files = glob.glob('talk_*.html')
speaker_files = glob.glob('speaker_*.html')

list_of_talks_16 = []
list_of_talks_17 = []
list_of_speakers = []

def sort_by_start_then_track(talk):
    return (talk['start_h'] * 100 + talk['start_m'], talk['track'])

def sort_by_name(speaker):
    return speaker['name']

def sanitize(s):
    s = s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', ' ').replace('\u2028', ' ').replace('“', '\\"').replace('”', '\\"').replace('’', "'").replace('‘', "'").replace('—', '-').replace('–', '-').replace('†', '').replace('\xe4', 'a').replace('\xe9', 'e').replace('\xc9', 'E').replace('\u0151', 'o').replace('\xa9', '').replace('\xab', '\\"').replace('\xbb', '\\"').replace('\u2009', ' ').replace('\xa0', ' ').replace('\xe8', 'e')

    # Make sure that we didn't miss any non ascii character.
    # This will raise an exception if there is still one.
    s.encode('ascii')

    return s

for name in files:
    with open(name) as f:
        yo = f.read()

    soup = bs4.BeautifulSoup(yo, 'lxml')

    title = soup.select_one('h3.talk-title').text
    m = re.search('»(.*)«', title)
    title = m.group(1)
    assert title

    names = [sanitize(x.text.strip()) for x in soup.select('div.name')]
    assert names

    abstract = soup.select_one('section.abstract').text.strip()
    detailed = soup.select_one('section.description').text.strip()

    time_place = soup.select_one('h3.talk-title > small').text.strip()
    m = re.match(
        r'2019-05-(16|17), (\d\d):(\d\d)–(\d\d):(\d\d), (.*)', time_place)
    date = m.group(1)
    assert date in ['16', '17']
    start_h = m.group(2)
    start_m = m.group(3)
    end_h = m.group(4)
    end_m = m.group(5)
    loc = m.group(6)
    if loc == 'IMAX Theatre':
        continue
    track = loc2track[loc]

    talk = {
        'title': sanitize(title),
        'names': names,
        'start_h': start_h,
        'start_m': start_m,
        'end_h': end_h,
        'end_m': end_m,
        'abstract': sanitize(abstract),
        'detailed': sanitize(detailed),
        'track': track,
    }
    if date == '16':
        list_of_talks_16.append(talk)
    else:
        list_of_talks_17.append(talk)

list_of_talks_16.sort(key=sort_by_start_then_track)
list_of_talks_17.sort(key=sort_by_start_then_track)

for name in speaker_files:
    with open(name) as f:
        yo = f.read()

    soup = bs4.BeautifulSoup(yo, 'lxml')
    speaker_name = soup.select_one('h3').text.strip().split('\n')[1].strip()
	
    bio = soup.select_one('div.speaker-bio').text.strip()

    list_of_speakers.append({
        'name': sanitize(speaker_name),
        'bio': sanitize(bio),
    })

list_of_speakers.sort(key=sort_by_name)

print('''
// THIS FILE HAS BEEN GENERATED USING gen-conf-structs.py !!!"1🤖!

struct talk {
    const char *title, *names, *abstract, *detailed;
    unsigned char start_h, start_m;
    unsigned char end_h, end_m;
    unsigned char track;
};

static const char *tracks[] = {
    "Conf 1",
    "Conf 2",
    "Workshop 1 (Big)",
    "Workshop 2 (Salon du president)",
    "Workshop 3 (classroom)",
    "Workshop 4 (classroom)",
};

struct speaker {
    const char *name, *bio;
};
''')


def t(tm):
    if tm[0] == '0':
        return tm[1]
    else:
        return tm


def gen_talk(talk):
    print('  {')
    print('    .title = "{}",'.format(talk['title']))
    print('    .names = "{}",'.format(', '.join(talk['names'])))
    print('    .abstract = "{}",'.format(talk['abstract']))
    print('    .detailed = "{}",'.format(talk['detailed']))
    print('    .start_h = {}, .start_m = {}, .end_h = {}, .end_m = {},'.format(
        t(talk['start_h']), t(talk['start_m']), t(talk['end_h']), t(talk['end_m'])))
    print('    .track = {},'.format(talk['track']))
    print('  },')


print('static struct talk talks_16[] = {')
for talk in list_of_talks_16:
    gen_talk(talk)
print('};')

print('static struct talk talks_17[] = {')
for talk in list_of_talks_17:
    gen_talk(talk)
print('};')

print('static struct speaker speakers[] = {')
for sp in list_of_speakers:
    print('{')
    print('    .name = "{}",'.format(sp['name']))
    print('    .bio = "{}",'.format(sp['bio']))
    print('},')
print('};')
