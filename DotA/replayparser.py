#!/usr/bin/python
import struct
import zlib

class Replay:
	header = {}
	players = []
	filename = ''
	
	def __init__(self, filename):
		self.filename = filename
		self.parseHeader()
		self.parseData()
		
	def parseHeader(self):
		try:
			f = open(self.filename, 'rb')
		except IOError:
			print '%s:file open failed.' % self.filename
			return
		buf = f.read(0x30)
		magicNum, hSize, fSize, Ver, dSize, numBlks = struct.unpack('28s5I', buf)
		self.header.update({'magicNUm':magicNum})
		self.header.update({'hSize':hSize})
		self.header.update({'Ver':Ver})
		self.header.update({'dSize':dSize})
		self.header.update({'numBlks':numBlks})
		if self.header['Ver'] == 1:
			subH = {}
			buf = f.read(0x14)
			vID, vNum, dNum, flags, length, chkSum = struct.unpack('4sI2H2I', buf)
			subH.update({'vID':vID})
			subH.update({'vNum':vNum})
			subH.update({'dNum':dNum})
			subH.update({'flags':flags})
			subH.update({'length':length})
			subH.update({'chkSum':chkSum})
			self.header.update({'subHeader':subH})
		f.close()
		
		
	def parseData(self):
		try:
			f = open(self.filename, 'rb')
		except IOError:
			print '%s:file open failed.' % self.filename
			return
			
		f.seek(0x44, 0)
		buf = f.read(0x08)
		cSize, dSize, unknown = struct.unpack('2HL', buf)
		print cSize
		cData = f.read(cSize)
		zObj = zlib.decompressobj()
		Data = zObj.decompress(cData)
		print "Decompress data:", dSize, "bytes"
		offset = 4
		
		rID, pID = struct.unpack('2B', Data[offset:offset+2])
		print rID, pID
		pName = ''
		offset = 6
		while Data[offset] != '\0':
			pName += Data[offset]
			offset += 1
		self.players.append(pName)
		
		offset += 3
		gName = ''
		while Data[offset] != '\0':
			gName += Data[offset]
			offset += 1
		print gName
		
		offset += 2
		dStr = ''
		pos = 0
		while Data[offset + pos] != '\0':
			if pos % 8 == 0:
				(mask,) = struct.unpack('B',Data[pos])
			else:
				if (mask & (2 ** (pos % 8))) == 0:
	  				dStr += chr(ord(Data[offset + pos]) - 1)
				else:
	  				dStr += Data[offset + pos]
			pos += 1
		
		mName = ''
		offset += (pos + 1)
		
		pos = 0x0c
		while dStr[pos] != '\0':
			mName += dStr[pos]
			pos += 1
		
		(pCnt, ) = struct.unpack('I', Data[offset:offset+4])
		print pCnt
		
		offset += 14
		while Data[offset] != '\0':
			pName = ''
			while Data[offset] != '\0':
				pName += Data[offset]
				offset += 1
			self.players.append(pName)
			offset += 9
		
		
		f.close()
	
	def printHeader(self):
		
		for pair in self.header:
			print pair, self.header[pair]

	def printInfo(self):
		if self.header['subHeader']['vID'] == 'PX3W':
			print 'WarCraft III Expansion Set'
			print 'v1.', self.header['subHeader']['vNum']
		for player in self.players:
			print player
	
	
		
			
			

r = Replay('C:/temp/LastReplay.w3g')
r.printInfo()
		