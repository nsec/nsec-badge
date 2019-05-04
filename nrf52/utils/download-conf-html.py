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

import bs4
import requests

# Download the data for the 2019 conference schedule.  Save talk pages as
# talk_*.html and speaker pages as speaker_*.html.  These files can then
# be processed with gen-conf-structs.py.

s = requests.Session()
yo = s.get('https://cfp.nsec.io/2019/schedule/').text
soup = bs4.BeautifulSoup(yo, 'lxml')

talk_divs = soup.find_all('div', attrs={'class': 'talk'})
for d in talk_divs:
        # Download the page of each talk.
	id_ = d['id']
	yo = s.get('https://cfp.nsec.io/2019/talk/{}'.format(id_)).text
	with open('talk_{}.html'.format(id_), 'w') as f:
		f.write(yo)

        # Download the page of each speaker of the talk.  If a speaker
        # participates in multiple talks, we will download the same page
        # multiple times, but it will still result in a single file (which is
        # fine).
	talk_soup = bs4.BeautifulSoup(yo, 'lxml')
	speaker_links = talk_soup.select('div.speaker > a')
	for sp in speaker_links:
		url = sp['href']
		yo = s.get('https://cfp.nsec.io{}'.format(url)).text
		id_ = url.split('/')[-1]
		with open('speaker_{}.html'.format(id_), 'w') as f:
			f.write(yo)
