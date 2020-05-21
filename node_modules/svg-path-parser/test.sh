./node_modules/pegjs/bin/pegjs grammar.peg parser.js 
# node -e "require('sys').puts(JSON.stringify(require('./parser').parse('M0, 0 ,-1, .2 3.45 -6. -7.89 1e2 -1e-2 -2.e-1'),null,2))"
# node -e "require('sys').puts(JSON.stringify(require('./parser').parse('M1,2 L3,4'),null,2))"
# node -e "require('sys').puts(JSON.stringify(require('./parser').parse('m1,2,8,9 L3,4,5-6 7 7'),null,2))"
# node -e "
# 	var path = 'M3,7 5-6 L1,7 1e2-.4 m-10,10 l10,0      \
#  V27 89 H23           v10 h10            \
#  C33,43 38,47 43,47   c0,5 5,10 10,10    \
#  S63,67 63,67         s-10,10 10,10      \
#  Q50,50 73,57         q20,-5 0,-10       \
#  T70,40               t0,-15             \
#  A5,5 45 1,0 40,20    a5,5 20 0,1 -10-10 \
#  Z'
# 	var cmds=require('./parser').parse(path);
# 	for (var i=0;i<cmds.length;++i) console.log(cmds[i]);
# "

# var d='M3,7 5-6 L1,7 1e2-.4 m-10,10 l10,0  \
#   V27 89 H23           v10 h10             \
#   C33,43 38,47 43,47   c0,5 5,10 10,10     \
#   S63,67 63,67         s-10,10 10,10       \
#   Q50,50 73,57         q20,-5 0,-10        \
#   T70,40               t0,-15              \
#   A5,5 45 1,0 40,20    a5,5 20 0,1 -10-10  Z'

# node -e "
# 	var path = 'M15.5,3.472222222222222c0,-0.7777777777777777,0.611111111111111,-1.4166666666666665,1.3888888888888888,-1.4166666666666665s1.4166666666666665,0.6388888888888888,1.4166666666666665,1.4166666666666665s-0.6388888888888888,1.4166666666666665,-1.4166666666666665,1.4166666666666665s-1.3888888888888888,-0.6388888888888888,-1.3888888888888888,-1.4166666666666665zM10,10 q60,60 100,0 q50,50 50,50 l40,0 l-40,40 l-100,-20zM15.5,-3.472222222222222c0,-0.7777777777777777,0.611111111111111,-1.4166666666666665,1.3888888888888888,-1.4166666666666665s1.4166666666666665,0.6388888888888888,1.4166666666666665,1.4166666666666665s-0.6388888888888888,1.4166666666666665,-1.4166666666666665,1.4166666666666665s-1.3888888888888888,-0.6388888888888888,-1.3888888888888888,-1.4166666666666665z';
# 	var cmds=require('./parser').parse(path);
# 	for (var i=0;i<cmds.length;++i) console.log(cmds[i]);
# "
# node -e "
# 	var path = 'M15.5,3.4722c0,-0.77,0.611,-1.41665,1.388,-1.41665s1.41665,0.6388,1.41665,1.41665s-0.6388,1.41665,-1.41665,1.41665s-1.388,-0.6388,-1.388,-1.41665zM10,10 q60,60 100,0 q50,50 50,50 l40,0 l-40,40 l-100,-20zM15.5,-3.4722c0,-0.77,0.611,-1.41665,1.388,-1.41665s1.41665,0.6388,1.41665,1.41665s-0.6388,1.41665,-1.41665,1.41665s-1.388,-0.6388,-1.388,-1.41665z';
# 	var cmds=require('./parser').parse(path);
# 	for (var i=0;i<cmds.length;++i) console.log(cmds[i]);
# "
node -e "
	var path = 'M1,2 L3,4 z M5,6 L7,8 z M9,1 L2,3 z';
	var cmds=require('./parser').parse(path);
	console.log(cmds.length);
	console.log(cmds);
"
# var path = 'M1,2 L3,4 z M5,6 L7,8 z M9,1 L2,3 z';
# var cmds=require('svg-path-parser').parse(path);
# console.log(cmds.length);
# // 6
# console.log(cmds);
# // [ { code: 'M', command: 'moveto', x: 1, y: 2 },
# //   { code: 'L', command: 'lineto', x: 3, y: 4 },
# //   { code: 'Z', command: 'closepath' },
# //   { code: 'M', command: 'moveto', x: 5, y: 6 },
# //   { code: 'L', command: 'lineto', x: 7, y: 8 },
# //   { code: 'Z', command: 'closepath' } ]
