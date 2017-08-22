from collections import Counter

inp = [map(lambda s: int(s, 16), line.split()) for line in file("output")]

freqOrder = "etaoinshrdlcumwfgypbvkjxqz ,.-"
letters = set("abcdefghijklmnopqrstuvwxyz ,.-'ABCDEFGHIJKLMNOPQRSTUVWXYZ")

#guess = [56, 141, 90, 38, 211, 166, 63, 80, 100, 159, 112, 127, 198, 71, 167, 154, 31, 99, 89, 18, 27, 167, 59, 6, 90, 78, 169, 71, 106, 106, 76, 30, 247, 159, 232, 161, 7, 62, 23]

guess = []

print len(guess), max(map(len, inp))

for i in range(len(guess), max(map(len, inp))):
  count = Counter([r[i] for r in inp if i < len(r)]);
  mcList = count.most_common()
  mcIdx = 0
  guess.append(0)

  done = False
  while not done:
    if (mcIdx >= len(mcList)): break
    mc = mcList[mcIdx][0]

    for l in freqOrder:
      guess[i] = mc ^ ord(l)
      def dec((i, c)):
        v = c ^ guess[i]
        return chr(v) if chr(v) in letters else "_"

      res = []
      for row in inp:
        res.append(map(dec, enumerate(row[0:i+1])))

      if all(map(lambda r : i >= len(r) or r[i] != "_", res)):
        print i, l
        done = True
        break
      else:
        mcIdx += 1


def dec((i, c)):
  v = c ^ guess[i]
  return chr(v) if chr(v) in letters else "_"

print guess
res = []
for row in inp:
  res.append(map(dec, enumerate(row)))

for row in res:
  print ''.join(row)
