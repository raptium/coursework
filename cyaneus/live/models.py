from appengine_django.models import BaseModel
from google.appengine.ext import db

class Game(BaseModel):
	name = db.StringProperty()
	def __str__(self):
		return self.name

class Team(BaseModel):
	name = db.StringProperty()
	#game = db.ReferenceProperty(Game)
	def __str__(self):
		return self.name


class Channel(BaseModel):
	name = db.StringProperty()
	def __str__(self):
		return self.name


class Match(BaseModel):
	game = db.ReferenceProperty(Game)
	begin_time = db.DateTimeProperty()
	teams = db.ListProperty(db.Key)
	channels = db.ListProperty(db.Key)
	def __str__(self):
		str = ''
		for team in self.teams.all():
			str += team.name + ' '
		return str


class Tool(BaseModel):
	name = db.StringProperty()
	def __str__(self):
		return self.name


class Method(BaseModel):
	channel = db.ReferenceProperty(Channel)
	tool = db.ReferenceProperty(Tool)
	reference = db.TextProperty()
	def __str__(self):
		return self.channel.name + ' ' + self.tool.name
