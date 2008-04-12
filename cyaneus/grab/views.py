# coding=utf-8
# import from google
from google.appengine.api import urlfetch

# import form django
from django.http import HttpResponse
from django.shortcuts import render_to_response

# import from standard python library
import htmllib, formatter

# import from cyaneus 
from admin.views import *

def getZhiBoBa(request):
	url = 'http://www.zhiboba.com'
	msg = 'Error'
	result = urlfetch.fetch(url)
	if result.status_code == 200:
		content = result.content
		msg = 'fetch OK.'
	content = content[content.find('class="tableBorder">') + 20:]
	content = content[:content.find('</table>')]
	content = content.replace('<img src="/images/tv.gif" border="0" onMouseover="','')
	content = content.replace('onMouseout="showChannels(false, \'\', \'\', null)"/>','')
	content = content.replace('<strong>','')
	content = content.replace('</strong>','')
	
	format = formatter.NullFormatter()
	parser = matchParser(format)
	parser.feed(content)
	parser.close()
	
	for match in parser.matches:
		query = Game.all().filter('name =', match['game'].decode('utf-8'))
		if query.count() == 0:
			game = Game(name=match['game'].decode('utf-8'))
			game.put()
		else:
			game = query[0]

		teams = []
		for steam in match['teams']:
			query = Team.all().filter('name =', steam.decode('utf-8'))
			if query.count() == 0:
				team = Team(name=steam.decode('utf-8'))
				team.put()
				teams.append(team.key())
			else:
				teams.append(query[0].key())
				
		schs = match['channels'].rsplit()
		channels = []
		for sch in schs:
			query = Channel.all().filter('name =', sch.decode('utf-8'))
			if query.count() == 0:
				channel = Channel(name=sch.decode('utf-8'))
				channel.put()
				channels.append(channel.key())
			else:
				channels.append(query[0].key())

		begin_time = datetime.strptime(match['begin_time'], '%Y %m %d %H:%M')
		query = Match.all().filter('game =', game).filter('begin_time =', begin_time)
		if query.count() > 0:
			for match in query:
				same = True
				for team_key in match.teams:
					for team in teams:
						diff = True
						if db.get(team_key).name == team:
							diff = False
					if diff:
						same = False
			if same:
					continue
		m = Match()
		m.game = game
		m.channels = channels
		m.teams = teams
		m.begin_time = begin_time
		m.put()
		msg = 'Match added'
	
	return HttpResponse(msg)
	
	
class matchParser(htmllib.HTMLParser):
	"""html parser can parse matches from zhiboba.com"""
	def __init__(self, formatter):
		htmllib.HTMLParser.__init__(self, formatter)
		self.matches = []
		self.tr = False
		self.tdN = 0
		self.match = {}
		self.teams = []
		self.date = ''
		self.time = ''
		
	def start_tr(self, attrs):
		self.tr = True
	
	def end_tr(self):
		if self.tr:
			self.tr = False
			self.tdN = 0
		
	def start_td(self, attrs):
		if self.tr:
			self.tdN += 1
	
	def end_td(self):
		pass
			
	def handle_data(self, data):
		if self.tr:
			if self.tdN == 1:
				if data.find('月') > 0:
					data = data.replace('月', ' ')
					data = data.replace('日', ' ')
					self.date = data
				return
			if self.tdN == 2:
				if self.match.has_key('begin_time'):
					return
				if data.find(':') > 0:
						self.time = data
				begin_time = '2008 ' + self.date + ' ' + self.time
				self.match.update({'begin_time':begin_time})
				return
			if self.tdN == 3:
				if self.match.has_key('game'):
					return
				self.match.update({'game':data})
				return
			if self.tdN == 4:
				if len(self.teams) > 0:
					return
				self.teams.append(data)
				return
			if self.tdN == 6:
				if self.match.has_key('teams'):
					return
				self.teams.append(data)
				self.match.update({'teams':self.teams})
				self.teams = []
				return
			if self.tdN == 7:
				if self.match.has_key('channels') or self.match == {}:
					return
				data = data[data.find('\', \'')+4:]
				data = data[:data.find('\', this')]
				data = data.replace('、', ' ')
				self.match.update({'channels':data})
				self.matches.append(self.match)
				self.match = {}
				return
