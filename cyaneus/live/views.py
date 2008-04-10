from django.shortcuts import render_to_response
from django.http import Http404
from google.appengine.ext import db
from live.models import *
import logging
import urllib

def live(request):
	games = Game.all()
	return render_to_response('live.html', {'games':games})

def listMatches(request, gameName):
	gameName = urllib.unquote(gameName).decode('utf-8')
	if gameName == 'all':
		m_matches = Match.all().order('begin_time')
	else:
		game = Game.all().filter('name =', gameName)[0]
		m_matches = Match.all().filter('game =', game).order('begin_time')
	
	matches = []
	for m in m_matches:
		match = {}
		match['begin_time'] = m.begin_time
		match['game'] = m.game.name
		match['teams'] = []
		for t in m.teams:
			match['teams'].append(db.get(t).name)
		match['channels'] = []
		for c in m.channels:
			match['channels'].append(db.get(c).name)
		matches.append(match)
	
	return render_to_response('matches.html', {'matches': matches})
	
def listMethods(request, ch):
	ch = urllib.unquote(ch).decode('utf-8')
	channel = Channel.all().filter('name =', ch)[0]
	m_methods = Method.all().filter('channel =', channel)
	methods = [];
	for m in m_methods:
		method = {}
		method['tool'] = m.tool.name
		method['channel'] = m.channel.name
		methods.append(method)
	return render_to_response('methods.html', {'methods': methods})
	
	
def actMethod(request, channel, tool):
	channel = urllib.unquote(channel).decode('utf-8')
	ch = Channel.all().filter('name =', channel)[0]
	t = Tool.all().filter('name =', tool)[0]
	method = Method.all().filter('channel =', ch).filter('tool =', t)[0]
	switch = {'SopCast': goSopCast,
		'TVANTS': goTVANTS
		}
	if t.name in switch:
	     return switch[t.name](method)
	else:
	     raise Http404
	
def goSopCast(method):
	address = method.reference
	return render_to_response('sopcast.html', {'address': address, 'name': method.channel.name})
	
	
def goTVANTS(method):
	return render_to_response('tvants.html', {})