from google.appengine.ext import db

class Game(db.Model):
	name = db.StringProperty()
	def __str__(self):
		return self.name

class Team(db.Model):
	name = db.StringProperty()
	#game = db.ReferenceProperty(Game)
	def __str__(self):
		return self.name


class Channel(db.Model):
	name = db.StringProperty()
	def __str__(self):
		return self.name


class Match(db.Model):
	game = db.ReferenceProperty(Game)
	begin_time = db.DateTimeProperty()
	teams = db.ListProperty(db.Key)
	channels = db.ListProperty(db.Key)
	def __str__(self):
		str = ''
		for team in self.teams.all():
			str += team.name + ' '
		return str


class Tool(db.Model):
	name = db.StringProperty()
	def __str__(self):
		return self.name


class Method(db.Model):
	channel = db.ReferenceProperty(Channel)
	tool = db.ReferenceProperty(Tool)
	reference = db.TextProperty()
	def __str__(self):
		return self.channel.name + ' ' + self.tool.name
