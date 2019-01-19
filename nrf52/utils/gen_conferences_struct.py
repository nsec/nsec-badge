#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import yaml
from bs4 import BeautifulSoup
from datetime import datetime


def print_html(_text):
    soup = BeautifulSoup(_text, features="html.parser")
    return soup.get_text().encode('utf-8')

def parse_speakers(yaml_out):
    """ _in: yaml
        _out: sorted list
    """
    speakers = {}
    for speaker in yaml_out:
        speakers['{} {}'.format(
            print_html(speaker['name']),
            print_html(speaker['surname'])
        )] = print_html(speaker['bio'])
    return speakers

def display_speakers(speakers):
    sys.stdout.write('const char * speakers[] = {\n')
    for speaker in sorted(speakers.iterkeys()):
        sys.stdout.write('   "{}",\n'.format(speaker))
    sys.stdout.write('};\n\n')

def display_speakers_bio(speakers_bio):
    sys.stdout.write('const char * speakers_bio[] = {\n')
    for speaker in sorted(speakers_bio.iterkeys()):
        sys.stdout.write('   "{}",\n'.format(speakers_bio[speaker]))
    sys.stdout.write('};\n\n')

def parse_date(yaml_in):
    """ _in: yaml
        _out: sorted list
    """
    date_dic = []
    for i in yaml_in:
        if 'date' in i:
            date_dic.append(datetime.strptime(i['date'], "%Y-%m-%d").strftime("%A, %B %d %Y"))
    return date_dic

def generate_speakers_struct(yaml_in):
    yaml_in = parse_speakers(yaml_in)
    display_speakers(yaml_in)
    display_speakers_bio(yaml_in)

def display_days_schedule_items(schedule):
    sys.stdout.write('static menu_item_s days_schedule_items[] = {\n')
    for date in schedule:
        sys.stdout.write('    {\n')
        sys.stdout.write('        .label = "{}",\n'.format(date))
        sys.stdout.write('        .handler = nsec_schedule_show_tracks\n')
        sys.stdout.write('    },\n')
    sys.stdout.write('    {\n')
    sys.stdout.write('        .label = "Presenters",\n')
    sys.stdout.write('        .handler = nsec_schedule_show_presenters\n')
    sys.stdout.write('    }\n')
    sys.stdout.write('};\n\n')

def generate_days_schedule_struct(yaml_in):
    yaml_in = parse_date(yaml_in)
    display_days_schedule_items(yaml_in)

def main(argument):
    # Read YAML file
    for filename in os.listdir(argument):
        if 'schedule.yml' in filename:
            with open(filename, 'r') as fd:
                generate_days_schedule_struct(yaml.load(fd))
        elif 'speakers.yml' in filename:
            # generate speakers and speakers bio
            with open(filename, 'r') as fd:
                generate_speakers_struct(yaml.load(fd))
        else:
            sys.exit('[-] schedule and speakers yaml files not found')

if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit('Usage: {} [YAML directory]'.format(sys.argv[0]))
    if os.path.isdir(sys.argv[1]):
        main(sys.argv[1])
