#pragma once

#include "Areas.h"
#include "Heightmap.h"


static glm::vec2 load_world0(Heightmap& heightmap, Areas& areas) {

	printf("Loading World 0\n");

	heightmap.load("./heightmaps/hmap_000_smooth.txt", 1.0);

	areas.clear();
	areas.add_wall(glm::vec2( 975, -975), glm::vec2( 975, 975), 20);
	areas.add_wall(glm::vec2( 975,  975), glm::vec2(-975, 975), 20);
	areas.add_wall(glm::vec2(-975,  975), glm::vec2(-975,-975), 20);
	areas.add_wall(glm::vec2(-975, -975), glm::vec2( 975,-975), 20);

	return glm::vec2(0, 0);
}

static glm::vec2 load_world1(Heightmap& heightmap, Areas& areas) {

	printf("Loading World 1\n");

	heightmap.load("./heightmaps/hmap_000_smooth.txt", 1.0);

	areas.clear();
	areas.add_crouch(glm::vec3(0,5,0), glm::vec2(1000.0f, 250.0f));
	areas.add_wall(glm::vec2( 975, -975), glm::vec2( 975, 975), 20);
	areas.add_wall(glm::vec2( 975,  975), glm::vec2(-975, 975), 20);
	areas.add_wall(glm::vec2(-975,  975), glm::vec2(-975,-975), 20);
	areas.add_wall(glm::vec2(-975, -975), glm::vec2( 975,-975), 20);

	return glm::vec2(0, 0);

}

static glm::vec2 load_world2(Heightmap& heightmap, Areas& areas) {

	printf("Loading World 2\n");

	heightmap.load("./heightmaps/hmap_004_smooth.txt", 1.0);

	areas.clear();
	areas.add_wall(glm::vec2(1013.78, -1023.47), glm::vec2( 1013.78,  1037.65), 20);
	areas.add_wall(glm::vec2(1013.78,  1037.65), glm::vec2(-1005.93,  1032.48), 20);
	areas.add_wall(glm::vec2(-1005.93, 1032.48), glm::vec2( -1012.46, -985.26), 20);
	areas.add_wall(glm::vec2(-1012.46, -985.26), glm::vec2( -680.57, -1001.82), 20);
	areas.add_wall(glm::vec2(-680.57, -1001.82), glm::vec2( -571.86, -1008.58), 20);
	areas.add_wall(glm::vec2(-571.86, -1008.58), glm::vec2( -441.50, -1025.14), 20);
	areas.add_wall(glm::vec2(-441.50, -1025.14), glm::vec2( -205.33, -1023.47), 20);
	areas.add_wall(glm::vec2(-205.33, -1023.47), glm::vec2( 1018.95, -1023.47), 20);

	return glm::vec2(0, 0);

}

static glm::vec2 load_world3(Heightmap& heightmap, Areas& areas) {

	printf("Loading World 3\n");

	heightmap.load("./heightmaps/hmap_007_smooth.txt", 1.0);

	areas.clear();
	areas.add_wall(glm::vec2(1137.99,  -2583.42), glm::vec2(1154.53,   2604.02), 20);
	areas.add_wall(glm::vec2(1154.53,   2604.02), glm::vec2(644.10,    2602.73), 20);
	areas.add_wall(glm::vec2(644.10,    2602.73), glm::vec2(504.73,    2501.38), 20);
	areas.add_wall(glm::vec2(504.73,    2501.38), glm::vec2(12.73,     2522.49), 20);
	areas.add_wall(glm::vec2(12.73,     2522.49), glm::vec2(-84.41,    2497.15), 20);
	areas.add_wall(glm::vec2(-84.41,    2497.15), glm::vec2(-342.03,   2481.34), 20);
	areas.add_wall(glm::vec2(-342.03,   2481.34), glm::vec2(-436.74,   2453.81), 20);
	areas.add_wall(glm::vec2(-436.74,   2453.81), glm::vec2(-555.85,   2480.54), 20);
	areas.add_wall(glm::vec2(-555.85,   2480.54), glm::vec2(-776.98,   2500.82), 20);
	areas.add_wall(glm::vec2(-776.98,   2500.82), glm::vec2(-877.50,   2466.82), 20);
	areas.add_wall(glm::vec2(-877.50,   2466.82), glm::vec2(-975.67,   2488.11), 20);
	areas.add_wall(glm::vec2(-975.67,   2488.11), glm::vec2(-995.97,   2607.62), 20);
	areas.add_wall(glm::vec2(-995.97,   2607.62), glm::vec2(-1142.54,  2612.13), 20);
	areas.add_wall(glm::vec2(-1142.54,  2612.13), glm::vec2(-1151.56,  2003.29), 20);
	areas.add_wall(glm::vec2(-1151.56,  2003.29), glm::vec2(-1133.52,  1953.68), 20);
	areas.add_wall(glm::vec2(-1133.52,  1953.68), glm::vec2(-1153.82,  1888.29), 20);
	areas.add_wall(glm::vec2(-1153.82,  1888.29), glm::vec2(-1151.56, -2608.12), 20);
	areas.add_wall(glm::vec2(-1151.56, -2608.12), glm::vec2(-1126.76, -2608.12), 20);
	areas.add_wall(glm::vec2(-1126.76, -2608.12), glm::vec2(-1133.52,  -427.57), 20);
	areas.add_wall(glm::vec2(-1133.52,  -427.57), glm::vec2(-1074.89,  -184.03), 20);
	areas.add_wall(glm::vec2(-1074.89,  -184.03), glm::vec2(-973.42,     48.23), 20);
	areas.add_wall(glm::vec2(-973.42,     48.23), glm::vec2(-928.32,    217.35), 20);
	areas.add_wall(glm::vec2(-928.32,    217.35), glm::vec2(-732.14,    535.30), 20);
	areas.add_wall(glm::vec2(-732.14,    535.30), glm::vec2(-734.39,    436.09), 20);
	areas.add_wall(glm::vec2(-734.39,    436.09), glm::vec2(-838.12,    167.75), 20);
	areas.add_wall(glm::vec2(-838.12,    167.75), glm::vec2(-937.34,   -427.57), 20);
	areas.add_wall(glm::vec2(-937.34,   -427.57), glm::vec2(-930.57,  -1164.94), 20);
	areas.add_wall(glm::vec2(-930.57,  -1164.94), glm::vec2(-844.88,  -1478.38), 20);
	areas.add_wall(glm::vec2(-844.88,  -1478.38), glm::vec2(-691.55,  -2166.15), 20);
	areas.add_wall(glm::vec2(-691.55,  -2166.15), glm::vec2(-648.70,  -2610.37), 20);
	areas.add_wall(glm::vec2(-648.70,  -2610.37), glm::vec2(1139.49,  -2581.06), 20);
	areas.add_wall(glm::vec2(-314.97,  -2472.82), glm::vec2(-258.59,  -2508.90), 20);
	areas.add_wall(glm::vec2(-258.59,  -2508.90), glm::vec2(-195.45,  -2504.39), 20);
	areas.add_wall(glm::vec2(-195.45,  -2504.39), glm::vec2(-199.96,  -2477.33), 20);
	areas.add_wall(glm::vec2(-199.96,  -2477.33), glm::vec2(-238.30,  -2450.27), 20);
	areas.add_wall(glm::vec2(-238.30,  -2450.27), glm::vec2(-281.14,  -2441.25), 20);
	areas.add_wall(glm::vec2(-281.14,  -2441.25), glm::vec2(-310.46,  -2466.06), 20);

	return glm::vec2(0, 0);

}

static glm::vec2 load_world4(Heightmap& heightmap, Areas& areas) {

	printf("Loading World 4\n");

	heightmap.load("./heightmaps/hmap_013_smooth.txt", 1.0);

	areas.clear();
	areas.add_wall(glm::vec2( 1225, -1000), glm::vec2( 1225, 1000), 20);
	areas.add_wall(glm::vec2( 1225,  1000), glm::vec2(-1225, 1000), 20);
	areas.add_wall(glm::vec2(-1225,  1000), glm::vec2(-1225,-1000), 20);
	areas.add_wall(glm::vec2(-1225, -1000), glm::vec2( 1225,-1000), 20);

	areas.add_jump(glm::vec3( 237.64, 5,  452.98), 75, 100);
	areas.add_jump(glm::vec3( 378.40, 5,  679.64), 75, 100);
	areas.add_jump(glm::vec3( 227.17, 5,  866.28), 75, 100);
	areas.add_jump(glm::vec3( -43.93, 5,  609.78), 75, 100);
	areas.add_jump(glm::vec3( 810.12, 5,  897.37), 75, 100);
	areas.add_jump(glm::vec3( 945.85, 5,  493.90), 75, 100);
	areas.add_jump(glm::vec3( 618.69, 5,  220.01), 75, 100);
	areas.add_jump(glm::vec3( 950.29, 5,  246.37), 75, 100);
	areas.add_jump(glm::vec3( 703.68, 5, -262.97), 75, 100);
	areas.add_jump(glm::vec3( 798.17, 5, -579.91), 75, 100);
	areas.add_jump(glm::vec3(1137.51, 5, -636.69), 75, 100);
	areas.add_jump(glm::vec3( 212.80, 5, -638.25), 75, 100);
	areas.add_jump(glm::vec3(  79.65, 5, -909.37), 75, 100);
	areas.add_jump(glm::vec3(-286.95, 5, -771.64), 75, 100);
	areas.add_jump(glm::vec3(-994.98, 5, -547.12), 75, 100);
	areas.add_jump(glm::vec3(-384.53, 5,  245.73), 75, 100);
	areas.add_jump(glm::vec3(-559.39, 5,  672.81), 75, 100);
	areas.add_jump(glm::vec3(-701.95, 5,  902.13), 75, 100);

	return glm::vec2(0, 0);

}

static glm::vec2 load_world5(Heightmap& heightmap, Areas& areas) {

	printf("Loading World 5\n");

	heightmap.load("./heightmaps/hmap_urban_001_smooth.txt", 1.0);

	areas.clear(); 
	areas.add_wall(glm::vec2(477.54,     762.20), glm::vec2(261.43,     980.61), 20);
	areas.add_wall(glm::vec2(450.08,     735.91), glm::vec2(236.42,     950.49), 20);
	areas.add_wall(glm::vec2(770.18,     811.10), glm::vec2(770.18,    1137.49), 20);
	areas.add_wall(glm::vec2(808.33,     811.10), glm::vec2(810.45,    1137.49), 20);
	areas.add_wall(glm::vec2(810.45,     804.10), glm::vec2(936.78,     799.44), 20);
	areas.add_wall(glm::vec2(936.78,     799.44), glm::vec2(939.11,     634.34), 20);
	areas.add_wall(glm::vec2(939.11,     634.34), glm::vec2(1286.05,    634.55), 20);
	areas.add_wall(glm::vec2(1286.05,    634.55), glm::vec2(1280.97,   1137.49), 20);
	areas.add_wall(glm::vec2(1280.97,   1137.49), glm::vec2(1149.56,   1146.81), 20);
	areas.add_wall(glm::vec2(1149.56,   1146.81), glm::vec2(1151.68,   1315.53), 20);
	areas.add_wall(glm::vec2(1151.68,   1315.53), glm::vec2(914.31,    1317.65), 20);
	areas.add_wall(glm::vec2(914.31,    1317.65), glm::vec2(911.97,    1142.15), 20);
	areas.add_wall(glm::vec2(911.97,    1142.15), glm::vec2(818.93,    1137.91), 20);
	areas.add_wall(glm::vec2(272.09,    1003.74), glm::vec2(401.21,    1137.59), 20);
	areas.add_wall(glm::vec2(401.21,    1137.59), glm::vec2(755.16,    1135.40), 20);
	areas.add_wall(glm::vec2(492.50,     772.82), glm::vec2(528.66,     798.22), 20);
	areas.add_wall(glm::vec2(528.66,     798.22), glm::vec2(765.94,     797.79), 20);
	areas.add_wall(glm::vec2(431.19,     713.73), glm::vec2(204.21,     482.63), 20);
	areas.add_wall(glm::vec2(204.21,     482.63), glm::vec2(350.53,     325.74), 20);
	areas.add_wall(glm::vec2(350.53,     325.74), glm::vec2(335.69,     310.90), 20);
	areas.add_wall(glm::vec2(335.69,     310.90), glm::vec2(-189.93,    840.77), 20);
	areas.add_wall(glm::vec2(-189.93,    840.77), glm::vec2(-172.98,    861.96), 20);
	areas.add_wall(glm::vec2(-172.98,    861.96), glm::vec2(-24.54,     709.28), 20);
	areas.add_wall(glm::vec2(-24.54,     709.28), glm::vec2(210.85,     940.50), 20);
	areas.add_wall(glm::vec2(-1084.78,   917.48), glm::vec2(-1082.57,  1027.92), 20);
	areas.add_wall(glm::vec2(-1082.57,  1027.92), glm::vec2(-1204.06,  1030.13), 20);
	areas.add_wall(glm::vec2(-1204.06,  1030.13), glm::vec2(-1201.85,   917.48), 20);
	areas.add_wall(glm::vec2(-1201.85,   917.48), glm::vec2(-1082.57,   917.48), 20);
	areas.add_wall(glm::vec2(-1089.20,   270.27), glm::vec2(-1086.99,   382.93), 20);
	areas.add_wall(glm::vec2(-1086.99,   382.93), glm::vec2(-1204.06,   387.34), 20);
	areas.add_wall(glm::vec2(-1204.06,   387.34), glm::vec2(-1204.06,   270.27), 20);
	areas.add_wall(glm::vec2(-1204.06,   270.27), glm::vec2(-1084.78,   272.48), 20);
	areas.add_wall(glm::vec2(1491.61,    265.82), glm::vec2(1494.18,    386.74), 20);
	areas.add_wall(glm::vec2(1494.18,    386.74), glm::vec2(1378.41,    386.74), 20);
	areas.add_wall(glm::vec2(1378.41,    386.74), glm::vec2(1378.41,    265.82), 20);
	areas.add_wall(glm::vec2(1378.41,    265.82), glm::vec2(1499.33,    270.96), 20);
	areas.add_wall(glm::vec2(1494.18,   -367.10), glm::vec2(1494.18,   -246.18), 20);
	areas.add_wall(glm::vec2(1494.18,   -246.18), glm::vec2(1375.83,   -246.18), 20);
	areas.add_wall(glm::vec2(1375.83,   -246.18), glm::vec2(1373.26,   -364.53), 20);
	areas.add_wall(glm::vec2(1373.26,   -364.53), glm::vec2(1499.33,   -364.53), 20);
	areas.add_wall(glm::vec2(-1088.90, -1665.86), glm::vec2(-1084.93, -1552.75), 20);
	areas.add_wall(glm::vec2(-1084.93, -1552.75), glm::vec2(-1202.01, -1552.75), 20);
	areas.add_wall(glm::vec2(-1202.01, -1552.75), glm::vec2(-1203.99, -1663.88), 20);
	areas.add_wall(glm::vec2(-1203.99, -1663.88), glm::vec2(-1086.91, -1663.88), 20);
	areas.add_wall(glm::vec2(-78.35,   -1210.59), glm::vec2(-94.14,   -1023.36), 20);
	areas.add_wall(glm::vec2(-94.14,   -1023.36), glm::vec2(-155.05,  -1032.38), 20);
	areas.add_wall(glm::vec2(-155.05,  -1032.38), glm::vec2(-179.86,   -820.34), 20);
	areas.add_wall(glm::vec2(-179.86,   -820.34), glm::vec2(-116.70,   -811.32), 20);
	areas.add_wall(glm::vec2(-116.70,   -811.32), glm::vec2(-89.63,   -1000.80), 20);
	areas.add_wall(glm::vec2(-89.63,   -1000.80), glm::vec2(-33.24,    -994.04), 20);
	areas.add_wall(glm::vec2(-33.24,    -994.04), glm::vec2(-12.93,   -1208.33), 20);
	areas.add_wall(glm::vec2(-12.93,   -1208.33), glm::vec2(-76.10,   -1212.84), 20);
	areas.add_wall(glm::vec2(-118.56,   -142.29), glm::vec2(-176.02,   -108.24), 20);
	areas.add_wall(glm::vec2(-176.02,   -108.24), glm::vec2(-278.18,   -291.27), 20);
	areas.add_wall(glm::vec2(-278.18,   -291.27), glm::vec2(-227.10,   -323.20), 20);
	areas.add_wall(glm::vec2(-227.10,   -323.20), glm::vec2(-114.30,   -140.16), 20);
	areas.add_wall(glm::vec2(-33.43,     -57.16), glm::vec2(153.87,      51.39), 20);
	areas.add_wall(glm::vec2(153.87,      51.39), glm::vec2(119.81,     104.60), 20);
	areas.add_wall(glm::vec2(119.81,     104.60), glm::vec2(-63.22,       0.31), 20);
	areas.add_wall(glm::vec2(-63.22,       0.31), glm::vec2(-29.17,     -52.90), 20);
	areas.add_wall(glm::vec2(-449.57,  -1219.96), glm::vec2(-443.12,  -1196.31), 20);
	areas.add_wall(glm::vec2(-443.12,  -1196.31), glm::vec2(-1201.49, -1192.57), 20);
	areas.add_wall(glm::vec2(-1201.49, -1192.57), glm::vec2(-1201.49, -1218.79), 20);
	areas.add_wall(glm::vec2(-1201.49, -1218.79), glm::vec2(-442.73,  -1218.74), 20);
	areas.add_wall(glm::vec2(996.02,     633.86), glm::vec2(1052.59,    633.86), 20);
	areas.add_wall(glm::vec2(1052.59,    633.86), glm::vec2(1067.35,    382.99), 20);
	areas.add_wall(glm::vec2(1067.35,    382.99), glm::vec2(1010.78,    382.99), 20);
	areas.add_wall(glm::vec2(1010.78,    382.99), glm::vec2(998.48,     638.78), 20);
	areas.add_wall(glm::vec2(1208.98,    633.86), glm::vec2(1213.90,    392.83), 20);
	areas.add_wall(glm::vec2(1213.90,    392.83), glm::vec2(1154.87,    390.37), 20);
	areas.add_wall(glm::vec2(1154.87,    390.37), glm::vec2(1157.33,    633.86), 20);
	areas.add_wall(glm::vec2(1157.33,    633.86), glm::vec2(1204.07,    633.86), 20);
	areas.add_wall(glm::vec2(-1217.82,   590.42), glm::vec2(-1173.38,   622.52), 20);
	areas.add_wall(glm::vec2(-1173.38,   622.52), glm::vec2(-1200.54,   671.91), 20);
	areas.add_wall(glm::vec2(-1200.54,   671.91), glm::vec2(-1267.21,   711.42), 20);
	areas.add_wall(glm::vec2(-1267.21,   711.42), glm::vec2(-1286.96,   664.50), 20);
	areas.add_wall(glm::vec2(-1286.96,   664.50), glm::vec2(-1274.62,   592.89), 20);
	areas.add_wall(glm::vec2(-1274.62,   592.89), glm::vec2(-1217.82,   592.89), 20);
	areas.add_wall(glm::vec2(861.99,     264.16), glm::vec2(852.86,     172.92), 20);
	areas.add_wall(glm::vec2(852.86,     172.92), glm::vec2(904.56,     142.51), 20);
	areas.add_wall(glm::vec2(904.56,     142.51), glm::vec2(971.47,     191.17), 20);
	areas.add_wall(glm::vec2(971.47,     191.17), glm::vec2(941.06,     273.28), 20);
	areas.add_wall(glm::vec2(941.06,     273.28), glm::vec2(865.03,     267.20), 20);
	areas.add_wall(glm::vec2(2.04,       273.54), glm::vec2(76.94,      344.69), 20);
	areas.add_wall(glm::vec2(76.94,      344.69), glm::vec2(222.99,     191.15), 20);
	areas.add_wall(glm::vec2(222.99,     191.15), glm::vec2(252.95,     217.36), 20);
	areas.add_wall(glm::vec2(252.95,     217.36), glm::vec2(-290.07,    756.64), 20);
	areas.add_wall(glm::vec2(-290.07,    756.64), glm::vec2(-308.80,    730.43), 20);
	areas.add_wall(glm::vec2(-308.80,    730.43), glm::vec2(-159.00,    580.63), 20);
	areas.add_wall(glm::vec2(-159.00,    580.63), glm::vec2(-226.41,    520.71), 20);
	areas.add_wall(glm::vec2(-226.41,    520.71), glm::vec2(9.53,       281.03), 20);
	areas.add_wall(glm::vec2(1494.76,   -496.66), glm::vec2(1496.73,   1442.38), 20);
	areas.add_wall(glm::vec2(1496.73,   1442.38), glm::vec2(1013.95,   1438.43), 20);
	areas.add_wall(glm::vec2(1013.95,   1438.43), glm::vec2(1011.97,   1468.10), 20);
	areas.add_wall(glm::vec2(1011.97,   1468.10), glm::vec2(1534.33,   1476.02), 20);
	areas.add_wall(glm::vec2(1534.33,   1476.02), glm::vec2(1534.33,   -465.01), 20);
	areas.add_wall(glm::vec2(1534.33,   -465.01), glm::vec2(1970.84,   -465.01), 20);
	areas.add_wall(glm::vec2(1970.84,   -465.01), glm::vec2(1971.08,   -340.88), 20);
	areas.add_wall(glm::vec2(1971.08,   -340.88), glm::vec2(1658.98,   -332.44), 20);
	areas.add_wall(glm::vec2(1658.98,   -332.44), glm::vec2(1662.94,   1602.65), 20);
	areas.add_wall(glm::vec2(1662.94,   1602.65), glm::vec2(1073.05,   1599.36), 20);
	areas.add_wall(glm::vec2(1073.05,   1599.36), glm::vec2(1066.82,   1585.85), 20);
	areas.add_wall(glm::vec2(1066.82,   1585.85), glm::vec2(1054.49,   1564.70), 20);
	areas.add_wall(glm::vec2(1054.49,   1564.70), glm::vec2(915.16,    1554.76), 20);
	areas.add_wall(glm::vec2(915.16,    1554.76), glm::vec2(911.60,    1608.03), 20);
	areas.add_wall(glm::vec2(911.60,    1608.03), glm::vec2(825.93,    1602.97), 20);
	areas.add_wall(glm::vec2(825.93,    1602.97), glm::vec2(-46.59,    1598.69), 20);
	areas.add_wall(glm::vec2(-46.59,    1598.69), glm::vec2(-48.57,    1248.48), 20);
	areas.add_wall(glm::vec2(-48.57,    1248.48), glm::vec2(-442.32,   1211.85), 20);
	areas.add_wall(glm::vec2(-442.32,   1211.85), glm::vec2(-434.40,   1610.57), 20);
	areas.add_wall(glm::vec2(-434.40,   1610.57), glm::vec2(-359.27,   1629.55), 20);
	areas.add_wall(glm::vec2(-359.27,   1629.55), glm::vec2(-374.44,   1759.60), 20);
	areas.add_wall(glm::vec2(-374.44,   1759.60), glm::vec2(-439.46,   1757.43), 20);
	areas.add_wall(glm::vec2(-439.46,   1757.43), glm::vec2(-483.22,   1764.33), 20);
	areas.add_wall(glm::vec2(-483.22,   1764.33), glm::vec2(-1118.99,  1750.84), 20);
	areas.add_wall(glm::vec2(-1118.99,  1750.84), glm::vec2(-1124.37,  1601.37), 20);
	areas.add_wall(glm::vec2(-1124.37,  1601.37), glm::vec2(-1774.60,  1599.21), 20);
	areas.add_wall(glm::vec2(-1774.60,  1599.21), glm::vec2(-1770.27, -1903.69), 20);
	areas.add_wall(glm::vec2(-1770.27, -1903.69), glm::vec2(-1646.72, -1901.88), 20);
	areas.add_wall(glm::vec2(-1646.72, -1901.88), glm::vec2(-1655.39,  1471.33), 20);
	areas.add_wall(glm::vec2(-1655.39,  1471.33), glm::vec2(-1122.20,  1471.33), 20);
	areas.add_wall(glm::vec2(-1122.20,  1471.33), glm::vec2(-1126.54,  1434.48), 20);
	areas.add_wall(glm::vec2(-1126.54,  1434.48), glm::vec2(-1612.04,  1438.82), 20);
	areas.add_wall(glm::vec2(-1612.04,  1438.82), glm::vec2(-1557.86,  1408.47), 20);
	areas.add_wall(glm::vec2(-1557.86,  1408.47), glm::vec2(-1534.02,  1339.11), 20);
	areas.add_wall(glm::vec2(-1534.02,  1339.11), glm::vec2(-1547.02,  1274.09), 20);
	areas.add_wall(glm::vec2(-1547.02,  1274.09), glm::vec2(-1605.54,  1241.58), 20);
	areas.add_wall(glm::vec2(-1605.54,  1241.58), glm::vec2(-1612.04,    29.98), 20);
	areas.add_wall(glm::vec2(-1612.04,    29.98), glm::vec2(-1577.37,    29.98), 20);
	areas.add_wall(glm::vec2(-1577.37,    29.98), glm::vec2(-1573.03,    68.99), 20);
	areas.add_wall(glm::vec2(-1573.03,    68.99), glm::vec2(-1521.01,    82.00), 20);
	areas.add_wall(glm::vec2(-1521.01,    82.00), glm::vec2(-1525.35,  -134.74), 20);
	areas.add_wall(glm::vec2(-1525.35,  -134.74), glm::vec2(-891.62,   -140.12), 20);
	areas.add_wall(glm::vec2(-891.62,   -140.12), glm::vec2(-665.51,     83.84), 20);
	areas.add_wall(glm::vec2(-665.51,     83.84), glm::vec2(-430.78,   -161.66), 20);
	areas.add_wall(glm::vec2(-430.78,   -161.66), glm::vec2(-646.13,   -381.31), 20);
	areas.add_wall(glm::vec2(-646.13,   -381.31), glm::vec2(-650.43,  -1029.50), 20);
	areas.add_wall(glm::vec2(-650.43,  -1029.50), glm::vec2(-448.01,  -1025.19), 20);
	areas.add_wall(glm::vec2(-448.01,  -1025.19), glm::vec2(-443.70,  -1059.64), 20);
	areas.add_wall(glm::vec2(-443.70,  -1059.64), glm::vec2(-1203.87, -1063.95), 20);
	areas.add_wall(glm::vec2(-1203.87, -1063.95), glm::vec2(-1203.87, -1025.19), 20);
	areas.add_wall(glm::vec2(-1203.87, -1025.19), glm::vec2(-1001.45, -1025.19), 20);
	areas.add_wall(glm::vec2(-1001.45, -1025.19), glm::vec2(-1001.45,  -824.92), 20);
	areas.add_wall(glm::vec2(-1001.45,  -824.92), glm::vec2(-1324.46,  -495.44), 20);
	areas.add_wall(glm::vec2(-1324.46,  -495.44), glm::vec2(-1524.73,  -486.83), 20);
	areas.add_wall(glm::vec2(-1524.73,  -486.83), glm::vec2(-1524.73,  -710.78), 20);
	areas.add_wall(glm::vec2(-1524.73,  -710.78), glm::vec2(-1574.26,  -708.63), 20);
	areas.add_wall(glm::vec2(-1574.26,  -708.63), glm::vec2(-1578.57,  -669.87), 20);
	areas.add_wall(glm::vec2(-1578.57,  -669.87), glm::vec2(-1604.41,  -667.72), 20);
	areas.add_wall(glm::vec2(-1604.41,  -667.72), glm::vec2(-1602.56, -1714.29), 20);
	areas.add_wall(glm::vec2(-1602.56, -1714.29), glm::vec2(-1544.11, -1750.90), 20);
	areas.add_wall(glm::vec2(-1544.11, -1750.90), glm::vec2(-1526.89, -1826.27), 20);
	areas.add_wall(glm::vec2(-1526.89, -1826.27), glm::vec2(-1524.73, -1908.10), 20);
	areas.add_wall(glm::vec2(-1524.73, -1908.10), glm::vec2(-1505.35, -1847.81), 20);
	areas.add_wall(glm::vec2(-1505.35, -1847.81), glm::vec2(-1429.98, -1856.42), 20);
	areas.add_wall(glm::vec2(-1429.98, -1856.42), glm::vec2(-1410.60, -1912.41), 20);
	areas.add_wall(glm::vec2(-1410.60, -1912.41), glm::vec2(-878.70,  -1918.87), 20);
	areas.add_wall(glm::vec2(-878.70,  -1918.87), glm::vec2(-863.63,  -1854.27), 20);
	areas.add_wall(glm::vec2(-863.63,  -1854.27), glm::vec2(-777.49,  -1854.27), 20);
	areas.add_wall(glm::vec2(-777.49,  -1854.27), glm::vec2(-762.41,  -1918.87), 20);
	areas.add_wall(glm::vec2(-762.41,  -1918.87), glm::vec2(-233.87,  -1915.62), 20);
	areas.add_wall(glm::vec2(-233.87,  -1915.62), glm::vec2(-227.54,  -1873.38), 20);
	areas.add_wall(glm::vec2(-227.54,  -1873.38), glm::vec2(-455.62,  -1871.27), 20);
	areas.add_wall(glm::vec2(-455.62,  -1871.27), glm::vec2(-451.39,  -1497.48), 20);
	areas.add_wall(glm::vec2(-451.39,  -1497.48), glm::vec2(-278.26,  -1502.82), 20);
	areas.add_wall(glm::vec2(-278.26,  -1502.82), glm::vec2(-276.28,  -1447.04), 20);
	areas.add_wall(glm::vec2(-276.28,  -1447.04), glm::vec2(-73.87,   -1453.75), 20);
	areas.add_wall(glm::vec2(-73.87,   -1453.75), glm::vec2(-71.27,   -1503.82), 20);
	areas.add_wall(glm::vec2(-71.27,   -1503.82), glm::vec2(-45.79,   -1879.72), 20);
	areas.add_wall(glm::vec2(-45.79,   -1879.72), glm::vec2(-121.95,  -1877.61), 20);
	areas.add_wall(glm::vec2(-121.95,  -1877.61), glm::vec2(-113.50,  -1924.07), 20);
	areas.add_wall(glm::vec2(-113.50,  -1924.07), glm::vec2(-115.61,  -2006.43), 20);
	areas.add_wall(glm::vec2(-115.61,  -2006.43), glm::vec2(629.86,   -2016.99), 20);
	areas.add_wall(glm::vec2(629.86,   -2016.99), glm::vec2(661.53,   -1909.28), 20);
	areas.add_wall(glm::vec2(661.53,   -1909.28), glm::vec2(765.01,   -1900.84), 20);
	areas.add_wall(glm::vec2(765.01,   -1900.84), glm::vec2(798.68,   -1926.36), 20);
	areas.add_wall(glm::vec2(798.68,   -1926.36), glm::vec2(1574.44,  -1920.78), 20);
	areas.add_wall(glm::vec2(1574.44,  -1920.78), glm::vec2(1616.07,  -1917.73), 20);
	areas.add_wall(glm::vec2(1616.07,  -1917.73), glm::vec2(1647.75,  -1913.51), 20);
	areas.add_wall(glm::vec2(1647.75,  -1913.51), glm::vec2(1654.08,  -1746.67), 20);
	areas.add_wall(glm::vec2(1654.08,  -1746.67), glm::vec2(1816.69,  -1748.79), 20);
	areas.add_wall(glm::vec2(1816.69,  -1748.79), glm::vec2(1812.47,  -1584.06), 20);
	areas.add_wall(glm::vec2(1812.47,  -1584.06), glm::vec2(2017.32,  -1590.40), 20);
	areas.add_wall(glm::vec2(2013.48,  -1595.35), glm::vec2(2005.79,  -1038.14), 20);
	areas.add_wall(glm::vec2(2005.79,  -1038.14), glm::vec2(1967.37,  -1030.45), 20);
	areas.add_wall(glm::vec2(1967.37,  -1030.45), glm::vec2(1975.05,   -895.95), 20);
	areas.add_wall(glm::vec2(1975.05,   -895.95), glm::vec2(1579.23,   -892.11), 20);
	areas.add_wall(glm::vec2(1579.23,   -892.11), glm::vec2(1579.23,   -849.84), 20);
	areas.add_wall(glm::vec2(1579.23,   -849.84), glm::vec2(1967.37,   -853.68), 20);
	areas.add_wall(glm::vec2(1967.37,   -853.68), glm::vec2(1955.84,   -500.13), 20);
	areas.add_wall(glm::vec2(1955.84,   -500.13), glm::vec2(1579.23,   -515.50), 20);
	areas.add_wall(glm::vec2(1579.23,   -515.50), glm::vec2(1575.39,   -496.29), 20);
	areas.add_wall(glm::vec2(1575.39,   -496.29), glm::vec2(1490.62,   -500.36), 20);

	return glm::vec2(200, 0);

}
