/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTrainTrackManager.cpp
*  PURPOSE:     Train Node Manager class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

std::map< unsigned int, SRailNode > CTrainTrackManager::m_OriginalTrainTrackData[4];

CTrainTrackManager::CTrainTrackManager ( )
{
    for ( int i = 0; i < NUM_RAILTRACKS; i++ )
    {
        // there are 4 train tracks so initialise them all
        m_pTrainTracks[i] = new CTrainTrack ( i, this, NULL, NULL );
    }
    m_fRailTrackLengths[0]      = 54278;
    m_dwNumberOfTrackNodes[0]   = 926;

    m_fRailTrackLengths[1]      = 15321;
    m_dwNumberOfTrackNodes[1]   = 67;

    m_fRailTrackLengths[2]      = 1945;
    m_dwNumberOfTrackNodes[2]   = 300;

    m_fRailTrackLengths[3]      = 26535;
    m_dwNumberOfTrackNodes[3]   = 656;

    m_OriginalTrainTrackData [0][0].Init ( 20443, 18338, 18, 0, 0 );

    m_OriginalTrainTrackData [0][1].Init ( 20472, 18224, 15, 44, 0 );

    m_OriginalTrainTrackData [0][2].Init ( 20508, 18117, 12, 86, 0 );

    m_OriginalTrainTrackData [0][3].Init ( 20555, 18020, 9, 126, 0 );

    m_OriginalTrainTrackData [0][4].Init ( 20608, 17929, 4, 166, 97 );

    m_OriginalTrainTrackData [0][5].Init ( 20670, 17848, 0, 204, 96 );

    m_OriginalTrainTrackData [0][6].Init ( 20736, 17770, -4, 242, 163 );

    m_OriginalTrainTrackData [0][7].Init ( 20812, 17698, -9, 282, 97 );

    m_OriginalTrainTrackData [0][8].Init ( 20888, 17633, -13, 319, 160 );

    m_OriginalTrainTrackData [0][9].Init ( 20970, 17568, -17, 358, 137 );

    m_OriginalTrainTrackData [0][10].Init ( 21056, 17508, -21, 398, 137 );

    m_OriginalTrainTrackData [0][11].Init ( 21142, 17450, -24, 437, 96 );

    m_OriginalTrainTrackData [0][12].Init ( 21229, 17392, -26, 476, 94 );

    m_OriginalTrainTrackData [0][13].Init ( 21316, 17336, -28, 515, 97 );

    m_OriginalTrainTrackData [0][14].Init ( 21408, 17275, -28, 556, 96 );

    m_OriginalTrainTrackData [0][15].Init ( 21491, 17220, -27, 593, 94 );

    m_OriginalTrainTrackData [0][16].Init ( 21574, 17160, -25, 632, 13 );

    m_OriginalTrainTrackData [0][17].Init ( 21655, 17100, -22, 670, 128 );

    m_OriginalTrainTrackData [0][18].Init ( 21733, 17035, -19, 708, 219 );

    m_OriginalTrainTrackData [0][19].Init ( 21808, 16967, -15, 746, 13 );

    m_OriginalTrainTrackData [0][20].Init ( 21880, 16893, -10, 784, 96 );

    m_OriginalTrainTrackData [0][21].Init ( 21949, 16814, -5, 824, 219 );

    m_OriginalTrainTrackData [0][22].Init ( 22011, 16728, 0, 863, 13 );

    m_OriginalTrainTrackData [0][23].Init ( 22070, 16635, 5, 905, 64 );

    m_OriginalTrainTrackData [0][24].Init ( 22121, 16535, 10, 947, 219 );

    m_OriginalTrainTrackData [0][25].Init ( 22167, 16428, 14, 991, 40 );

    m_OriginalTrainTrackData [0][26].Init ( 22206, 16309, 19, 1037, 254 );

    m_OriginalTrainTrackData [0][27].Init ( 22236, 16184, 22, 1086, 179 );

    m_OriginalTrainTrackData [0][28].Init ( 22240, 16162, 23, 1094, 13 );

    m_OriginalTrainTrackData [0][29].Init ( 22261, 16046, 25, 1138, 232 );

    m_OriginalTrainTrackData [0][30].Init ( 22265, 16000, 26, 1156, 169 );

    m_OriginalTrainTrackData [0][31].Init ( 22274, 15895, 27, 1195, 13 );

    m_OriginalTrainTrackData [0][32].Init ( 22277, 15819, 28, 1224, 222 );

    m_OriginalTrainTrackData [0][33].Init ( 22279, 15758, 28, 1247, 13 );

    m_OriginalTrainTrackData [0][34].Init ( 22279, 15719, 28, 1261, 13 );

    m_OriginalTrainTrackData [0][35].Init ( 22278, 15695, 28, 1270, 254 );

    m_OriginalTrainTrackData [0][36].Init ( 22276, 15672, 28, 1279, 13 );

    m_OriginalTrainTrackData [0][37].Init ( 22268, 15642, 28, 1290, 1 );

    m_OriginalTrainTrackData [0][38].Init ( 22261, 15620, 28, 1299, 96 );

    m_OriginalTrainTrackData [0][39].Init ( 22252, 15591, 28, 1310, 219 );

    m_OriginalTrainTrackData [0][40].Init ( 22248, 15565, 28, 1320, 1 );

    m_OriginalTrainTrackData [0][41].Init ( 22248, 15548, 28, 1327, 128 );

    m_OriginalTrainTrackData [0][42].Init ( 22248, 15419, 28, 1375, 219 );

    m_OriginalTrainTrackData [0][43].Init ( 22248, 15318, 33, 1413, 148 );

    m_OriginalTrainTrackData [0][44].Init ( 22248, 15307, 34, 1417, 32 );

    m_OriginalTrainTrackData [0][45].Init ( 22248, 15193, 43, 1460, 219 );

    m_OriginalTrainTrackData [0][46].Init ( 22248, 15088, 55, 1499, 1 );

    m_OriginalTrainTrackData [0][47].Init ( 22248, 14986, 65, 1537, 64 );

    m_OriginalTrainTrackData [0][48].Init ( 22248, 14919, 71, 1563, 219 );

    m_OriginalTrainTrackData [0][49].Init ( 22248, 14826, 76, 1597, 1 );

    m_OriginalTrainTrackData [0][50].Init ( 22248, 14756, 78, 1624, 96 );

    m_OriginalTrainTrackData [0][51].Init ( 22248, 14679, 78, 1653, 219 );

    m_OriginalTrainTrackData [0][52].Init ( 22248, 14601, 78, 1682, 1 );

    m_OriginalTrainTrackData [0][53].Init ( 22248, 14529, 78, 1709, 96 );

    m_OriginalTrainTrackData [0][54].Init ( 22248, 14470, 78, 1731, 219 );

    m_OriginalTrainTrackData [0][55].Init ( 22247, 14392, 78, 1760, 1 );

    m_OriginalTrainTrackData [0][56].Init ( 22247, 14309, 78, 1791, 128 );

    m_OriginalTrainTrackData [0][57].Init ( 22247, 14218, 78, 1825, 219 );

    m_OriginalTrainTrackData [0][58].Init ( 22247, 14148, 78, 1852, 1 );

    m_OriginalTrainTrackData [0][59].Init ( 22247, 14065, 78, 1883, 96 );

    m_OriginalTrainTrackData [0][60].Init ( 22247, 13950, 78, 1926, 219 );

    m_OriginalTrainTrackData [0][61].Init ( 22247, 13878, 78, 1953, 238 );

    m_OriginalTrainTrackData [0][62].Init ( 22247, 13825, 78, 1973, 64 );

    m_OriginalTrainTrackData [0][63].Init ( 22247, 13780, 78, 1990, 219 );

    m_OriginalTrainTrackData [0][64].Init ( 22247, 13746, 78, 2002, 238 );

    m_OriginalTrainTrackData [0][65].Init ( 22247, 13728, 78, 2009, 64 );

    m_OriginalTrainTrackData [0][66].Init ( 22247, 13690, 78, 2023, 219 );

    m_OriginalTrainTrackData [0][67].Init ( 22247, 13658, 78, 2035, 1 );

    m_OriginalTrainTrackData [0][68].Init ( 22247, 13628, 78, 2047, 128 );

    m_OriginalTrainTrackData [0][69].Init ( 22247, 13600, 78, 2057, 219 );

    m_OriginalTrainTrackData [0][70].Init ( 22248, 13570, 78, 2068, 1 );

    m_OriginalTrainTrackData [0][71].Init ( 22248, 13526, 78, 2085, 96 );

    m_OriginalTrainTrackData [0][72].Init ( 22248, 13485, 78, 2100, 219 );

    m_OriginalTrainTrackData [0][73].Init ( 22248, 13440, 78, 2117, 1 );

    m_OriginalTrainTrackData [0][74].Init ( 22248, 13398, 78, 2133, 128 );

    m_OriginalTrainTrackData [0][75].Init ( 22248, 13360, 78, 2147, 219 );

    m_OriginalTrainTrackData [0][76].Init ( 22248, 13330, 78, 2158, 1 );

    m_OriginalTrainTrackData [0][77].Init ( 22248, 13289, 78, 2174, 96 );

    m_OriginalTrainTrackData [0][78].Init ( 22248, 13269, 78, 2181, 219 );

    m_OriginalTrainTrackData [0][79].Init ( 22249, 13235, 78, 2194, 1 );

    m_OriginalTrainTrackData [0][80].Init ( 22250, 13201, 78, 2207, 96 );

    m_OriginalTrainTrackData [0][81].Init ( 22254, 13171, 78, 2218, 219 );

    m_OriginalTrainTrackData [0][82].Init ( 22261, 13142, 78, 2229, 1 );

    m_OriginalTrainTrackData [0][83].Init ( 22277, 13114, 78, 2242, 96 );

    m_OriginalTrainTrackData [0][84].Init ( 22300, 13081, 78, 2257, 219 );

    m_OriginalTrainTrackData [0][85].Init ( 22337, 13033, 78, 2279, 1 );

    m_OriginalTrainTrackData [0][86].Init ( 22395, 12967, 78, 2312, 96 );

    m_OriginalTrainTrackData [0][87].Init ( 22445, 12906, 78, 2342, 219 );

    m_OriginalTrainTrackData [0][88].Init ( 22488, 12854, 78, 2367, 1 );

    m_OriginalTrainTrackData [0][89].Init ( 22535, 12795, 78, 2395, 96 );

    m_OriginalTrainTrackData [0][90].Init ( 22589, 12737, 78, 2425, 219 );

    m_OriginalTrainTrackData [0][91].Init ( 22639, 12674, 78, 2455, 1 );

    m_OriginalTrainTrackData [0][92].Init ( 22685, 12608, 78, 2485, 96 );

    m_OriginalTrainTrackData [0][93].Init ( 22725, 12538, 78, 2516, 219 );

    m_OriginalTrainTrackData [0][94].Init ( 22763, 12467, 78, 2546, 1 );

    m_OriginalTrainTrackData [0][95].Init ( 22796, 12394, 78, 2576, 96 );

    m_OriginalTrainTrackData [0][96].Init ( 22825, 12321, 78, 2605, 219 );

    m_OriginalTrainTrackData [0][97].Init ( 22850, 12248, 78, 2634, 1 );

    m_OriginalTrainTrackData [0][98].Init ( 22870, 12176, 78, 2662, 128 );

    m_OriginalTrainTrackData [0][99].Init ( 22888, 12105, 78, 2690, 219 );

    m_OriginalTrainTrackData [0][100].Init ( 22901, 12035, 78, 2717, 230 );

    m_OriginalTrainTrackData [0][101].Init ( 22910, 11969, 78, 2742, 64 );

    m_OriginalTrainTrackData [0][102].Init ( 22915, 11907, 78, 2765, 179 );

    m_OriginalTrainTrackData [0][103].Init ( 22918, 11865, 78, 2781, 1 );

    m_OriginalTrainTrackData [0][104].Init ( 22918, 11545, 78, 2901, 254 );

    m_OriginalTrainTrackData [0][105].Init ( 22918, 10714, 78, 3212, 219 );

    m_OriginalTrainTrackData [0][106].Init ( 22918, 10201, 78, 3405, 1 );

    m_OriginalTrainTrackData [0][107].Init ( 22918, 9913, 78, 3513, 254 );

    m_OriginalTrainTrackData [0][108].Init ( 22918, 9785, 78, 3561, 2 );

    m_OriginalTrainTrackData [0][109].Init ( 22904, 9704, 77, 3591, 1 );

    m_OriginalTrainTrackData [0][110].Init ( 22877, 9613, 77, 3627, 243 );

    m_OriginalTrainTrackData [0][111].Init ( 22839, 9507, 77, 3669, 189 );

    m_OriginalTrainTrackData [0][112].Init ( 22791, 9390, 77, 3717, 238 );

    m_OriginalTrainTrackData [0][113].Init ( 22734, 9265, 78, 3768, 128 );

    m_OriginalTrainTrackData [0][114].Init ( 22669, 9133, 78, 3823, 172 );

    m_OriginalTrainTrackData [0][115].Init ( 22598, 9000, 78, 3880, 238 );

    m_OriginalTrainTrackData [0][116].Init ( 22523, 8866, 78, 3938, 64 );

    m_OriginalTrainTrackData [0][117].Init ( 22444, 8735, 78, 3995, 172 );

    m_OriginalTrainTrackData [0][118].Init ( 22255, 8480, 87, 4114, 238 );

    m_OriginalTrainTrackData [0][119].Init ( 22191, 8391, 83, 4155, 96 );

    m_OriginalTrainTrackData [0][120].Init ( 22145, 8307, 82, 4191, 182 );

    m_OriginalTrainTrackData [0][121].Init ( 22121, 8233, 79, 4220, 1 );

    m_OriginalTrainTrackData [0][122].Init ( 22118, 8186, 79, 4238, 128 );

    m_OriginalTrainTrackData [0][123].Init ( 22118, 7866, 79, 4358, 182 );

    m_OriginalTrainTrackData [0][124].Init ( 22118, 5946, 79, 5078, 148 );

    m_OriginalTrainTrackData [0][125].Init ( 22122, 4229, 61, 5722, 32 );

    m_OriginalTrainTrackData [0][126].Init ( 22123, 3782, 58, 5889, 182 );

    m_OriginalTrainTrackData [0][127].Init ( 22119, 3542, 55, 5979, 110 );

    m_OriginalTrainTrackData [0][128].Init ( 22119, 3259, 53, 6085, 96 );

    m_OriginalTrainTrackData [0][129].Init ( 22123, 3030, 53, 6171, 182 );

    m_OriginalTrainTrackData [0][130].Init ( 22122, 2795, 53, 6259, 97 );

    m_OriginalTrainTrackData [0][131].Init ( 22121, 2586, 55, 6338, 160 );

    m_OriginalTrainTrackData [0][132].Init ( 22122, 2362, 58, 6422, 182 );

    m_OriginalTrainTrackData [0][133].Init ( 22138, 2253, 58, 6463, 1 );

    m_OriginalTrainTrackData [0][134].Init ( 22168, 2141, 63, 6507, 96 );

    m_OriginalTrainTrackData [0][135].Init ( 22250, 1916, 68, 6596, 182 );

    m_OriginalTrainTrackData [0][136].Init ( 22344, 1668, 83, 6696, 1 );

    m_OriginalTrainTrackData [0][137].Init ( 22412, 1448, 109, 6782, 96 );

    m_OriginalTrainTrackData [0][138].Init ( 22445, 1342, 124, 6824, 182 );

    m_OriginalTrainTrackData [0][139].Init ( 22484, 1229, 139, 6869, 1 );

    m_OriginalTrainTrackData [0][140].Init ( 22510, 1134, 153, 6906, 96 );

    m_OriginalTrainTrackData [0][141].Init ( 22535, 1026, 168, 6947, 182 );

    m_OriginalTrainTrackData [0][142].Init ( 22565, 842, 192, 7017, 1 );

    m_OriginalTrainTrackData [0][143].Init ( 22587, 682, 203, 7078, 32 );

    m_OriginalTrainTrackData [0][144].Init ( 22607, 513, 208, 7141, 182 );

    m_OriginalTrainTrackData [0][145].Init ( 22620, 290, 215, 7225, 1 );

    m_OriginalTrainTrackData [0][146].Init ( 22624, -643, 262, 7575, 96 );

    m_OriginalTrainTrackData [0][147].Init ( 22626, -741, 263, 7612, 73 );

    m_OriginalTrainTrackData [0][148].Init ( 22622, -834, 265, 7647, 1 );

    m_OriginalTrainTrackData [0][149].Init ( 22615, -896, 263, 7670, 32 );

    m_OriginalTrainTrackData [0][150].Init ( 22601, -961, 261, 7695, 73 );

    m_OriginalTrainTrackData [0][151].Init ( 22578, -1037, 256, 7725, 1 );

    m_OriginalTrainTrackData [0][152].Init ( 22542, -1129, 249, 7762, 160 );

    m_OriginalTrainTrackData [0][153].Init ( 22501, -1225, 241, 7801, 86 );

    m_OriginalTrainTrackData [0][154].Init ( 22455, -1333, 232, 7845, 110 );

    m_OriginalTrainTrackData [0][155].Init ( 22397, -1450, 220, 7894, 128 );

    m_OriginalTrainTrackData [0][156].Init ( 22344, -1557, 209, 7939, 182 );

    m_OriginalTrainTrackData [0][157].Init ( 22283, -1667, 197, 7986, 1 );

    m_OriginalTrainTrackData [0][158].Init ( 22218, -1786, 184, 8037, 173 );

    m_OriginalTrainTrackData [0][159].Init ( 22155, -1906, 170, 8088, 182 );

    m_OriginalTrainTrackData [0][160].Init ( 22104, -2001, 159, 8128, 148 );

    m_OriginalTrainTrackData [0][161].Init ( 22071, -2061, 152, 8154, 96 );

    m_OriginalTrainTrackData [0][162].Init ( 22042, -2108, 146, 8175, 182 );

    m_OriginalTrainTrackData [0][163].Init ( 22002, -2157, 141, 8198, 161 );

    m_OriginalTrainTrackData [0][164].Init ( 21955, -2196, 136, 8221, 160 );

    m_OriginalTrainTrackData [0][165].Init ( 21902, -2223, 132, 8243, 182 );

    m_OriginalTrainTrackData [0][166].Init ( 21842, -2244, 129, 8267, 1 );

    m_OriginalTrainTrackData [0][167].Init ( 21764, -2271, 124, 8298, 64 );

    m_OriginalTrainTrackData [0][168].Init ( 21562, -2321, 115, 8376, 182 );

    m_OriginalTrainTrackData [0][169].Init ( 21378, -2349, 107, 8446, 1 );

    m_OriginalTrainTrackData [0][170].Init ( 21193, -2362, 102, 8516, 96 );

    m_OriginalTrainTrackData [0][171].Init ( 20998, -2358, 100, 8589, 182 );

    m_OriginalTrainTrackData [0][172].Init ( 20804, -2342, 103, 8662, 1 );

    m_OriginalTrainTrackData [0][173].Init ( 20668, -2325, 108, 8713, 128 );

    m_OriginalTrainTrackData [0][174].Init ( 20533, -2301, 113, 8765, 219 );

    m_OriginalTrainTrackData [0][175].Init ( 20397, -2280, 117, 8816, 13 );

    m_OriginalTrainTrackData [0][176].Init ( 20263, -2262, 122, 8867, 96 );

    m_OriginalTrainTrackData [0][177].Init ( 20136, -2245, 125, 8915, 219 );

    m_OriginalTrainTrackData [0][178].Init ( 20062, -2236, 128, 8943, 13 );

    m_OriginalTrainTrackData [0][179].Init ( 19998, -2227, 129, 8967, 128 );

    m_OriginalTrainTrackData [0][180].Init ( 19740, -2199, 135, 9064, 219 );

    m_OriginalTrainTrackData [0][181].Init ( 19622, -2191, 137, 9109, 32 );

    m_OriginalTrainTrackData [0][182].Init ( 19492, -2182, 139, 9158, 96 );

    m_OriginalTrainTrackData [0][183].Init ( 19378, -2174, 140, 9200, 219 );

    m_OriginalTrainTrackData [0][184].Init ( 19255, -2175, 140, 9247, 13 );

    m_OriginalTrainTrackData [0][185].Init ( 19146, -2183, 146, 9288, 128 );

    m_OriginalTrainTrackData [0][186].Init ( 19031, -2206, 151, 9332, 219 );

    m_OriginalTrainTrackData [0][187].Init ( 18925, -2245, 166, 9374, 32 );

    m_OriginalTrainTrackData [0][188].Init ( 18826, -2287, 179, 9414, 32 );

    m_OriginalTrainTrackData [0][189].Init ( 18728, -2340, 197, 9456, 219 );

    m_OriginalTrainTrackData [0][190].Init ( 18633, -2400, 216, 9498, 32 );

    m_OriginalTrainTrackData [0][191].Init ( 18542, -2462, 235, 9539, 32 );

    m_OriginalTrainTrackData [0][192].Init ( 18446, -2526, 255, 9583, 219 );

    m_OriginalTrainTrackData [0][193].Init ( 18356, -2587, 273, 9624, 13 );

    m_OriginalTrainTrackData [0][194].Init ( 18261, -2649, 290, 9666, 128 );

    m_OriginalTrainTrackData [0][195].Init ( 18172, -2700, 300, 9705, 219 );

    m_OriginalTrainTrackData [0][196].Init ( 18071, -2750, 312, 9747, 13 );

    m_OriginalTrainTrackData [0][197].Init ( 17983, -2784, 320, 9782, 96 );

    m_OriginalTrainTrackData [0][198].Init ( 17883, -2816, 330, 9822, 219 );

    m_OriginalTrainTrackData [0][199].Init ( 17800, -2835, 342, 9853, 13 );

    m_OriginalTrainTrackData [0][200].Init ( 17707, -2851, 354, 9889, 96 );

    m_OriginalTrainTrackData [0][201].Init ( 17539, -2863, 381, 9952, 219 );

    m_OriginalTrainTrackData [0][202].Init ( 17377, -2862, 408, 10013, 32 );

    m_OriginalTrainTrackData [0][203].Init ( 17218, -2854, 431, 10072, 96 );

    m_OriginalTrainTrackData [0][204].Init ( 17057, -2850, 448, 10133, 81 );

    m_OriginalTrainTrackData [0][205].Init ( 16883, -2859, 465, 10198, 32 );

    m_OriginalTrainTrackData [0][206].Init ( 16787, -2871, 475, 10234, 173 );

    m_OriginalTrainTrackData [0][207].Init ( 16694, -2891, 485, 10270, 219 );

    m_OriginalTrainTrackData [0][208].Init ( 16599, -2918, 496, 10307, 32 );

    m_OriginalTrainTrackData [0][209].Init ( 16503, -2953, 507, 10346, 160 );

    m_OriginalTrainTrackData [0][210].Init ( 16412, -2999, 517, 10384, 219 );

    m_OriginalTrainTrackData [0][211].Init ( 16324, -3051, 527, 10422, 13 );

    m_OriginalTrainTrackData [0][212].Init ( 16244, -3121, 534, 10462, 96 );

    m_OriginalTrainTrackData [0][213].Init ( 16171, -3194, 541, 10501, 219 );

    m_OriginalTrainTrackData [0][214].Init ( 16109, -3288, 544, 10543, 32 );

    m_OriginalTrainTrackData [0][215].Init ( 16057, -3387, 547, 10585, 96 );

    m_OriginalTrainTrackData [0][216].Init ( 16015, -3491, 549, 10627, 219 );

    m_OriginalTrainTrackData [0][217].Init ( 15984, -3602, 551, 10670, 238 );

    m_OriginalTrainTrackData [0][218].Init ( 15962, -3698, 555, 10707, 96 );

    m_OriginalTrainTrackData [0][219].Init ( 15948, -3814, 559, 10751, 182 );

    m_OriginalTrainTrackData [0][220].Init ( 15945, -3915, 563, 10789, 1 );

    m_OriginalTrainTrackData [0][221].Init ( 15957, -4023, 569, 10829, 254 );

    m_OriginalTrainTrackData [0][222].Init ( 15982, -4131, 571, 10871, 246 );

    m_OriginalTrainTrackData [0][223].Init ( 16018, -4231, 573, 10911, 1 );

    m_OriginalTrainTrackData [0][224].Init ( 16077, -4336, 571, 10956, 64 );

    m_OriginalTrainTrackData [0][225].Init ( 16143, -4437, 568, 11001, 219 );

    m_OriginalTrainTrackData [0][226].Init ( 16237, -4546, 562, 11055, 41 );

    m_OriginalTrainTrackData [0][227].Init ( 16336, -4643, 555, 11107, 232 );

    m_OriginalTrainTrackData [0][228].Init ( 16456, -4748, 541, 11167, 219 );

    m_OriginalTrainTrackData [0][229].Init ( 16560, -4840, 529, 11219, 1 );

    m_OriginalTrainTrackData [0][230].Init ( 16670, -4932, 511, 11273, 222 );

    m_OriginalTrainTrackData [0][231].Init ( 16767, -5013, 496, 11320, 246 );

    m_OriginalTrainTrackData [0][232].Init ( 16872, -5097, 477, 11371, 1 );

    m_OriginalTrainTrackData [0][233].Init ( 16976, -5180, 458, 11421, 232 );

    m_OriginalTrainTrackData [0][234].Init ( 17087, -5266, 438, 11473, 219 );

    m_OriginalTrainTrackData [0][235].Init ( 17194, -5348, 416, 11524, 217 );

    m_OriginalTrainTrackData [0][236].Init ( 17342, -5459, 392, 11593, 222 );

    m_OriginalTrainTrackData [0][237].Init ( 17429, -5525, 375, 11634, 86 );

    m_OriginalTrainTrackData [0][238].Init ( 17480, -5564, 368, 11658, 57 );

    m_OriginalTrainTrackData [0][239].Init ( 17690, -5720, 339, 11756, 160 );

    m_OriginalTrainTrackData [0][240].Init ( 17790, -5813, 324, 11808, 123 );

    m_OriginalTrainTrackData [0][241].Init ( 17922, -5934, 306, 11875, 57 );

    m_OriginalTrainTrackData [0][242].Init ( 18081, -6163, 278, 11979, 96 );

    m_OriginalTrainTrackData [0][243].Init ( 18182, -6405, 256, 12078, 22 );

    m_OriginalTrainTrackData [0][244].Init ( 18239, -6658, 235, 12175, 233 );

    m_OriginalTrainTrackData [0][245].Init ( 18267, -6918, 220, 12273, 98 );

    m_OriginalTrainTrackData [0][246].Init ( 18279, -7182, 204, 12372, 160 );

    m_OriginalTrainTrackData [0][247].Init ( 18279, -7422, 205, 12462, 140 );

    m_OriginalTrainTrackData [0][248].Init ( 18279, -7682, 206, 12559, 100 );

    m_OriginalTrainTrackData [0][249].Init ( 18279, -7940, 207, 12656, 99 );

    m_OriginalTrainTrackData [0][250].Init ( 18279, -8182, 208, 12747, 204 );

    m_OriginalTrainTrackData [0][251].Init ( 18279, -8422, 208, 12837, 97 );

    m_OriginalTrainTrackData [0][252].Init ( 18279, -8682, 208, 12934, 220 );

    m_OriginalTrainTrackData [0][253].Init ( 18279, -9100, 206, 13091, 144 );

    m_OriginalTrainTrackData [0][254].Init ( 18279, -9292, 205, 13163, 98 );

    m_OriginalTrainTrackData [0][255].Init ( 18279, -9488, 198, 13237, 218 );

    m_OriginalTrainTrackData [0][256].Init ( 18279, -9744, 185, 13333, 172 );

    m_OriginalTrainTrackData [0][257].Init ( 18279, -10256, 185, 13525, 105 );

    m_OriginalTrainTrackData [0][258].Init ( 18279, -10384, 185, 13573, 224 );

    m_OriginalTrainTrackData [0][259].Init ( 18279, -10512, 185, 13621, 159 );

    m_OriginalTrainTrackData [0][260].Init ( 18279, -10640, 185, 13669, 2 );

    m_OriginalTrainTrackData [0][261].Init ( 18279, -10768, 185, 13717, 67 );

    m_OriginalTrainTrackData [0][262].Init ( 18279, -10896, 185, 13765, 202 );

    m_OriginalTrainTrackData [0][263].Init ( 18279, -10976, 185, 13795, 160 );

    m_OriginalTrainTrackData [0][264].Init ( 18278, -11032, 184, 13816, 16 );

    m_OriginalTrainTrackData [0][265].Init ( 18279, -11072, 184, 13831, 68 );

    m_OriginalTrainTrackData [0][266].Init ( 18279, -11112, 184, 13846, 10 );

    m_OriginalTrainTrackData [0][267].Init ( 18279, -11168, 184, 13867, 60 );

    m_OriginalTrainTrackData [0][268].Init ( 18275, -11295, 184, 13914, 192 );

    m_OriginalTrainTrackData [0][269].Init ( 18267, -11423, 183, 13962, 97 );

    m_OriginalTrainTrackData [0][270].Init ( 18252, -11549, 181, 14010, 16 );

    m_OriginalTrainTrackData [0][271].Init ( 18231, -11675, 178, 14058, 189 );

    m_OriginalTrainTrackData [0][272].Init ( 18205, -11792, 175, 14103, 94 );

    m_OriginalTrainTrackData [0][273].Init ( 18194, -11832, 174, 14118, 159 );

    m_OriginalTrainTrackData [0][274].Init ( 18185, -11872, 172, 14134, 1 );

    m_OriginalTrainTrackData [0][275].Init ( 18175, -11912, 172, 14149, 67 );

    m_OriginalTrainTrackData [0][276].Init ( 18163, -11952, 171, 14165, 146 );

    m_OriginalTrainTrackData [0][277].Init ( 18132, -12042, 166, 14201, 48 );

    m_OriginalTrainTrackData [0][278].Init ( 18086, -12159, 160, 14248, 208 );

    m_OriginalTrainTrackData [0][279].Init ( 18048, -12247, 154, 14284, 254 );

    m_OriginalTrainTrackData [0][280].Init ( 18002, -12361, 151, 14330, 201 );

    m_OriginalTrainTrackData [0][281].Init ( 17954, -12479, 145, 14378, 54 );

    m_OriginalTrainTrackData [0][282].Init ( 17856, -12716, 133, 14474, 12 );

    m_OriginalTrainTrackData [0][283].Init ( 17810, -12829, 126, 14520, 10 );

    m_OriginalTrainTrackData [0][284].Init ( 17759, -12946, 121, 14567, 9 );

    m_OriginalTrainTrackData [0][285].Init ( 17713, -13079, 116, 14620, 191 );

    m_OriginalTrainTrackData [0][286].Init ( 17701, -13125, 115, 14638, 65 );

    m_OriginalTrainTrackData [0][287].Init ( 17689, -13171, 114, 14656, 159 );

    m_OriginalTrainTrackData [0][288].Init ( 17676, -13218, 113, 14674, 2 );

    m_OriginalTrainTrackData [0][289].Init ( 17663, -13264, 111, 14692, 245 );

    m_OriginalTrainTrackData [0][290].Init ( 17634, -13373, 108, 14734, 248 );

    m_OriginalTrainTrackData [0][291].Init ( 17613, -13486, 105, 14777, 48 );

    m_OriginalTrainTrackData [0][292].Init ( 17595, -13621, 101, 14829, 0 );

    m_OriginalTrainTrackData [0][293].Init ( 17589, -13762, 99, 14881, 128 );

    m_OriginalTrainTrackData [0][294].Init ( 17588, -13810, 99, 14899, 144 );

    m_OriginalTrainTrackData [0][295].Init ( 17588, -13858, 99, 14917, 66 );

    m_OriginalTrainTrackData [0][296].Init ( 17588, -13906, 99, 14935, 129 );

    m_OriginalTrainTrackData [0][297].Init ( 17589, -13954, 99, 14953, 202 );

    m_OriginalTrainTrackData [0][298].Init ( 17589, -14082, 99, 15001, 4 );

    m_OriginalTrainTrackData [0][299].Init ( 17589, -14898, 102, 15307, 189 );

    m_OriginalTrainTrackData [0][300].Init ( 17589, -14931, 102, 15320, 5 );

    m_OriginalTrainTrackData [0][301].Init ( 17589, -15059, 102, 15368, 159 );

    m_OriginalTrainTrackData [0][302].Init ( 17589, -15115, 102, 15389, 251 );

    m_OriginalTrainTrackData [0][303].Init ( 17589, -15155, 102, 15404, 33 );

    m_OriginalTrainTrackData [0][304].Init ( 17589, -15195, 102, 15419, 204 );

    m_OriginalTrainTrackData [0][305].Init ( 17590, -15251, 102, 15440, 48 );

    m_OriginalTrainTrackData [0][306].Init ( 17583, -15306, 102, 15461, 208 );

    m_OriginalTrainTrackData [0][307].Init ( 17571, -15354, 102, 15479, 254 );

    m_OriginalTrainTrackData [0][308].Init ( 17550, -15404, 102, 15500, 11 );

    m_OriginalTrainTrackData [0][309].Init ( 17526, -15447, 102, 15518, 123 );

    m_OriginalTrainTrackData [0][310].Init ( 17493, -15490, 102, 15538, 94 );

    m_OriginalTrainTrackData [0][311].Init ( 17458, -15525, 102, 15557, 46 );

    m_OriginalTrainTrackData [0][312].Init ( 17409, -15564, 102, 15580, 20 );

    m_OriginalTrainTrackData [0][313].Init ( 17366, -15589, 102, 15599, 189 );

    m_OriginalTrainTrackData [0][314].Init ( 17316, -15610, 102, 15619, 101 );

    m_OriginalTrainTrackData [0][315].Init ( 17268, -15623, 102, 15638, 159 );

    m_OriginalTrainTrackData [0][316].Init ( 17215, -15630, 102, 15658, 249 );

    m_OriginalTrainTrackData [0][317].Init ( 17166, -15630, 102, 15676, 199 );

    m_OriginalTrainTrackData [0][318].Init ( 17069, -15630, 102, 15713, 16 );

    m_OriginalTrainTrackData [0][319].Init ( 16941, -15630, 102, 15761, 48 );

    m_OriginalTrainTrackData [0][320].Init ( 16813, -15630, 102, 15809, 75 );

    m_OriginalTrainTrackData [0][321].Init ( 16557, -15630, 102, 15905, 65 );

    m_OriginalTrainTrackData [0][322].Init ( 16429, -15630, 102, 15953, 153 );

    m_OriginalTrainTrackData [0][323].Init ( 16173, -15630, 102, 16049, 144 );

    m_OriginalTrainTrackData [0][324].Init ( 15917, -15630, 102, 16145, 126 );

    m_OriginalTrainTrackData [0][325].Init ( 15653, -15630, 101, 16244, 183 );

    m_OriginalTrainTrackData [0][326].Init ( 15517, -15630, 102, 16295, 160 );

    m_OriginalTrainTrackData [0][327].Init ( 15389, -15630, 102, 16343, 170 );

    m_OriginalTrainTrackData [0][328].Init ( 15024, -15630, 102, 16480, 9 );

    m_OriginalTrainTrackData [0][329].Init ( 14733, -15631, 100, 16589, 141 );

    m_OriginalTrainTrackData [0][330].Init ( 13800, -15630, 100, 16939, 87 );

    m_OriginalTrainTrackData [0][331].Init ( 13575, -15629, 100, 17023, 65 );

    m_OriginalTrainTrackData [0][332].Init ( 11994, -15630, 100, 17616, 190 );

    m_OriginalTrainTrackData [0][333].Init ( 11875, -15630, 100, 17661, 221 );

    m_OriginalTrainTrackData [0][334].Init ( 11780, -15632, 100, 17696, 208 );

    m_OriginalTrainTrackData [0][335].Init ( 11684, -15632, 100, 17732, 127 );

    m_OriginalTrainTrackData [0][336].Init ( 11588, -15630, 100, 17768, 206 );

    m_OriginalTrainTrackData [0][337].Init ( 11491, -15626, 97, 17805, 123 );

    m_OriginalTrainTrackData [0][338].Init ( 11392, -15618, 94, 17842, 121 );

    m_OriginalTrainTrackData [0][339].Init ( 11290, -15604, 89, 17880, 57 );

    m_OriginalTrainTrackData [0][340].Init ( 11187, -15585, 81, 17920, 142 );

    m_OriginalTrainTrackData [0][341].Init ( 11080, -15558, 69, 17961, 0 );

    m_OriginalTrainTrackData [0][342].Init ( 10970, -15523, 58, 18004, 136 );

    m_OriginalTrainTrackData [0][343].Init ( 10855, -15480, 41, 18050, 132 );

    m_OriginalTrainTrackData [0][344].Init ( 10737, -15426, 20, 18099, 8 );

    m_OriginalTrainTrackData [0][345].Init ( 10675, -15390, 10, 18126, 0 );

    m_OriginalTrainTrackData [0][346].Init ( 10607, -15342, 0, 18157, 17 );

    m_OriginalTrainTrackData [0][347].Init ( 10533, -15280, -8, 18193, 16 );

    m_OriginalTrainTrackData [0][348].Init ( 10454, -15209, -16, 18233, 250 );

    m_OriginalTrainTrackData [0][349].Init ( 10372, -15128, -22, 18276, 0 );

    m_OriginalTrainTrackData [0][350].Init ( 10286, -15038, -28, 18323, 234 );

    m_OriginalTrainTrackData [0][351].Init ( 10197, -14941, -30, 18373, 10 );

    m_OriginalTrainTrackData [0][352].Init ( 10104, -14839, -32, 18424, 19 );

    m_OriginalTrainTrackData [0][353].Init ( 10011, -14732, -35, 18477, 0 );

    m_OriginalTrainTrackData [0][354].Init ( 9796, -14486, -40, 18600, 224 );

    m_OriginalTrainTrackData [0][355].Init ( 9581, -14238, -42, 18723, 222 );

    m_OriginalTrainTrackData [0][356].Init ( 9365, -13985, -43, 18848, 247 );

    m_OriginalTrainTrackData [0][357].Init ( 9147, -13732, -43, 18973, 0 );

    m_OriginalTrainTrackData [0][358].Init ( 8927, -13476, -42, 19100, 249 );

    m_OriginalTrainTrackData [0][359].Init ( 8704, -13220, -40, 19227, 250 );

    m_OriginalTrainTrackData [0][360].Init ( 8480, -12963, -37, 19355, 1 );

    m_OriginalTrainTrackData [0][361].Init ( 8252, -12706, -34, 19484, 0 );

    m_OriginalTrainTrackData [0][362].Init ( 8021, -12449, -31, 19613, 224 );

    m_OriginalTrainTrackData [0][363].Init ( 7787, -12195, -27, 19743, 2 );

    m_OriginalTrainTrackData [0][364].Init ( 7550, -11943, -25, 19872, 7 );

    m_OriginalTrainTrackData [0][365].Init ( 7309, -11693, -22, 20003, 0 );

    m_OriginalTrainTrackData [0][366].Init ( 7063, -11447, -21, 20133, 7 );

    m_OriginalTrainTrackData [0][367].Init ( 6814, -11205, -20, 20263, 3 );

    m_OriginalTrainTrackData [0][368].Init ( 6688, -11088, -21, 20328, 3 );

    m_OriginalTrainTrackData [0][369].Init ( 6559, -10967, -21, 20394, 0 );

    m_OriginalTrainTrackData [0][370].Init ( 6519, -10932, -21, 20414, 11 );

    m_OriginalTrainTrackData [0][371].Init ( 6393, -10821, -20, 20477, 247 );

    m_OriginalTrainTrackData [0][372].Init ( 6225, -10669, -20, 20562, 101 );

    m_OriginalTrainTrackData [0][373].Init ( 6203, -10647, -20, 20574, 0 );

    m_OriginalTrainTrackData [0][374].Init ( 5840, -10340, -13, 20752, 7 );

    m_OriginalTrainTrackData [0][375].Init ( 5472, -10045, -1, 20929, 0 );

    m_OriginalTrainTrackData [0][376].Init ( 5098, -9766, 14, 21104, 4 );

    m_OriginalTrainTrackData [0][377].Init ( 4716, -9502, 32, 21278, 0 );

    m_OriginalTrainTrackData [0][378].Init ( 4327, -9256, 53, 21450, 0 );

    m_OriginalTrainTrackData [0][379].Init ( 3931, -9032, 74, 21621, 3 );

    m_OriginalTrainTrackData [0][380].Init ( 3523, -8828, 95, 21792, 7 );

    m_OriginalTrainTrackData [0][381].Init ( 3108, -8647, 116, 21962, 0 );

    m_OriginalTrainTrackData [0][382].Init ( 2683, -8491, 134, 22132, 131 );

    m_OriginalTrainTrackData [0][383].Init ( 2248, -8362, 150, 22302, 136 );

    m_OriginalTrainTrackData [0][384].Init ( 1802, -8261, 161, 22473, 133 );

    m_OriginalTrainTrackData [0][385].Init ( 1345, -8190, 168, 22647, 0 );

    m_OriginalTrainTrackData [0][386].Init ( 876, -8151, 169, 22823, 248 );

    m_OriginalTrainTrackData [0][387].Init ( 718, -8148, 167, 22883, 133 );

    m_OriginalTrainTrackData [0][388].Init ( 396, -8144, 163, 23003, 131 );

    m_OriginalTrainTrackData [0][389].Init ( 302, -8144, 160, 23039, 0 );

    m_OriginalTrainTrackData [0][390].Init ( 208, -8144, 159, 23074, 11 );

    m_OriginalTrainTrackData [0][391].Init ( 114, -8144, 159, 23109, 10 );

    m_OriginalTrainTrackData [0][392].Init ( 20, -8144, 159, 23144, 247 );

    m_OriginalTrainTrackData [0][393].Init ( -72, -8143, 158, 23179, 0 );

    m_OriginalTrainTrackData [0][394].Init ( -166, -8144, 156, 23214, 132 );

    m_OriginalTrainTrackData [0][395].Init ( -260, -8145, 154, 23249, 227 );

    m_OriginalTrainTrackData [0][396].Init ( -354, -8144, 150, 23285, 227 );

    m_OriginalTrainTrackData [0][397].Init ( -448, -8144, 145, 23320, 0 );

    m_OriginalTrainTrackData [0][398].Init ( -543, -8144, 137, 23355, 231 );

    m_OriginalTrainTrackData [0][399].Init ( -637, -8144, 127, 23391, 234 );

    m_OriginalTrainTrackData [0][400].Init ( -730, -8144, 114, 23426, 235 );

    m_OriginalTrainTrackData [0][401].Init ( -853, -8149, 98, 23472, 0 );

    m_OriginalTrainTrackData [0][402].Init ( -979, -8164, 88, 23519, 238 );

    m_OriginalTrainTrackData [0][403].Init ( -1106, -8188, 82, 23568, 243 );

    m_OriginalTrainTrackData [0][404].Init ( -1234, -8220, 81, 23617, 240 );

    m_OriginalTrainTrackData [0][405].Init ( -1361, -8260, 84, 23667, 0 );

    m_OriginalTrainTrackData [0][406].Init ( -1484, -8308, 89, 23717, 239 );

    m_OriginalTrainTrackData [0][407].Init ( -1601, -8362, 98, 23765, 228 );

    m_OriginalTrainTrackData [0][408].Init ( -1711, -8423, 108, 23812, 241 );

    m_OriginalTrainTrackData [0][409].Init ( -1813, -8488, 120, 23858, 0 );

    m_OriginalTrainTrackData [0][410].Init ( -1903, -8559, 132, 23900, 246 );

    m_OriginalTrainTrackData [0][411].Init ( -1980, -8634, 146, 23941, 239 );

    m_OriginalTrainTrackData [0][412].Init ( -2042, -8713, 159, 23978, 238 );

    m_OriginalTrainTrackData [0][413].Init ( -2098, -8795, 172, 24016, 0 );

    m_OriginalTrainTrackData [0][414].Init ( -2159, -8883, 187, 24056, 248 );

    m_OriginalTrainTrackData [0][415].Init ( -2224, -8975, 202, 24098, 243 );

    m_OriginalTrainTrackData [0][416].Init ( -2291, -9070, 217, 24142, 221 );

    m_OriginalTrainTrackData [0][417].Init ( -2362, -9166, 233, 24186, 0 );

    m_OriginalTrainTrackData [0][418].Init ( -2432, -9263, 248, 24231, 144 );

    m_OriginalTrainTrackData [0][419].Init ( -2503, -9359, 263, 24276, 144 );

    m_OriginalTrainTrackData [0][420].Init ( -2573, -9451, 276, 24319, 144 );

    m_OriginalTrainTrackData [0][421].Init ( -2641, -9539, 289, 24361, 0 );

    m_OriginalTrainTrackData [0][422].Init ( -2705, -9622, 299, 24400, 149 );

    m_OriginalTrainTrackData [0][423].Init ( -2765, -9698, 308, 24437, 38 );

    m_OriginalTrainTrackData [0][424].Init ( -2820, -9765, 314, 24469, 30 );

    m_OriginalTrainTrackData [0][425].Init ( -2878, -9824, 319, 24500, 0 );

    m_OriginalTrainTrackData [0][426].Init ( -2950, -9880, 322, 24535, 66 );

    m_OriginalTrainTrackData [0][427].Init ( -3034, -9931, 325, 24571, 30 );

    m_OriginalTrainTrackData [0][428].Init ( -3129, -9976, 326, 24611, 71 );

    m_OriginalTrainTrackData [0][429].Init ( -3230, -10012, 328, 24651, 0 );

    m_OriginalTrainTrackData [0][430].Init ( -3336, -10039, 328, 24692, 90 );

    m_OriginalTrainTrackData [0][431].Init ( -3446, -10055, 329, 24734, 95 );

    m_OriginalTrainTrackData [0][432].Init ( -3555, -10060, 329, 24775, 96 );

    m_OriginalTrainTrackData [0][433].Init ( -3662, -10051, 328, 24815, 0 );

    m_OriginalTrainTrackData [0][434].Init ( -3765, -10027, 328, 24855, 87 );

    m_OriginalTrainTrackData [0][435].Init ( -3861, -9988, 328, 24893, 27 );

    m_OriginalTrainTrackData [0][436].Init ( -3948, -9932, 328, 24932, 145 );

    m_OriginalTrainTrackData [0][437].Init ( -4027, -9872, 328, 24969, 0 );

    m_OriginalTrainTrackData [0][438].Init ( -4092, -9821, 328, 25000, 146 );

    m_OriginalTrainTrackData [0][439].Init ( -4149, -9778, 328, 25027, 147 );

    m_OriginalTrainTrackData [0][440].Init ( -4199, -9740, 328, 25051, 149 );

    m_OriginalTrainTrackData [0][441].Init ( -4245, -9704, 328, 25073, 0 );

    m_OriginalTrainTrackData [0][442].Init ( -4292, -9668, 328, 25095, 37 );

    m_OriginalTrainTrackData [0][443].Init ( -4342, -9629, 328, 25119, 34 );

    m_OriginalTrainTrackData [0][444].Init ( -4398, -9584, 328, 25146, 68 );

    m_OriginalTrainTrackData [0][445].Init ( -4465, -9531, 328, 25178, 0 );

    m_OriginalTrainTrackData [0][446].Init ( -4543, -9466, 328, 25216, 69 );

    m_OriginalTrainTrackData [0][447].Init ( -4639, -9388, 328, 25262, 44 );

    m_OriginalTrainTrackData [0][448].Init ( -4752, -9293, 328, 25317, 92 );

    m_OriginalTrainTrackData [0][449].Init ( -4876, -9197, 331, 25376, 0 );

    m_OriginalTrainTrackData [0][450].Init ( -4998, -9119, 339, 25430, 29 );

    m_OriginalTrainTrackData [0][451].Init ( -5116, -9058, 351, 25480, 71 );

    m_OriginalTrainTrackData [0][452].Init ( -5231, -9012, 366, 25527, 29 );

    m_OriginalTrainTrackData [0][453].Init ( -5343, -8982, 384, 25570, 0 );

    m_OriginalTrainTrackData [0][454].Init ( -5452, -8966, 403, 25612, 254 );

    m_OriginalTrainTrackData [0][455].Init ( -5558, -8964, 422, 25651, 254 );

    m_OriginalTrainTrackData [0][456].Init ( -5659, -8974, 441, 25689, 4 );

    m_OriginalTrainTrackData [0][457].Init ( -5757, -8996, 458, 25727, 0 );

    m_OriginalTrainTrackData [0][458].Init ( -5851, -9028, 472, 25764, 12 );

    m_OriginalTrainTrackData [0][459].Init ( -5940, -9071, 482, 25801, 11 );

    m_OriginalTrainTrackData [0][460].Init ( -6026, -9122, 488, 25839, 24 );

    m_OriginalTrainTrackData [0][461].Init ( -6105, -9175, 492, 25874, 0 );

    m_OriginalTrainTrackData [0][462].Init ( -6177, -9229, 496, 25908, 14 );

    m_OriginalTrainTrackData [0][463].Init ( -6243, -9283, 501, 25940, 13 );

    m_OriginalTrainTrackData [0][464].Init ( -6303, -9341, 506, 25972, 3 );

    m_OriginalTrainTrackData [0][465].Init ( -6356, -9404, 513, 26002, 0 );

    m_OriginalTrainTrackData [0][466].Init ( -6404, -9474, 520, 26034, 255 );

    m_OriginalTrainTrackData [0][467].Init ( -6447, -9554, 530, 26068, 5 );

    m_OriginalTrainTrackData [0][468].Init ( -6485, -9646, 540, 26106, 5 );

    m_OriginalTrainTrackData [0][469].Init ( -6520, -9750, 553, 26147, 0 );

    m_OriginalTrainTrackData [0][470].Init ( -6551, -9869, 567, 26193, 223 );

    m_OriginalTrainTrackData [0][471].Init ( -6579, -10007, 584, 26246, 18 );

    m_OriginalTrainTrackData [0][472].Init ( -6605, -10163, 602, 26305, 19 );

    m_OriginalTrainTrackData [0][473].Init ( -6628, -10329, 622, 26368, 0 );

    m_OriginalTrainTrackData [0][474].Init ( -6636, -10392, 630, 26392, 17 );

    m_OriginalTrainTrackData [0][475].Init ( -6650, -10492, 642, 26430, 22 );

    m_OriginalTrainTrackData [0][476].Init ( -6670, -10652, 662, 26490, 17 );

    m_OriginalTrainTrackData [0][477].Init ( -6693, -10807, 680, 26549, 0 );

    m_OriginalTrainTrackData [0][478].Init ( -6715, -10958, 697, 26606, 14 );

    m_OriginalTrainTrackData [0][479].Init ( -6743, -11101, 713, 26661, 15 );

    m_OriginalTrainTrackData [0][480].Init ( -6776, -11237, 728, 26713, 16 );

    m_OriginalTrainTrackData [0][481].Init ( -6814, -11364, 740, 26763, 0 );

    m_OriginalTrainTrackData [0][482].Init ( -6860, -11482, 750, 26810, 20 );

    m_OriginalTrainTrackData [0][483].Init ( -6915, -11590, 758, 26856, 2 );

    m_OriginalTrainTrackData [0][484].Init ( -6981, -11686, 762, 26899, 6 );

    m_OriginalTrainTrackData [0][485].Init ( -7058, -11770, 764, 26942, 0 );

    m_OriginalTrainTrackData [0][486].Init ( -7149, -11838, 761, 26985, 2 );

    m_OriginalTrainTrackData [0][487].Init ( -7254, -11892, 754, 27029, 7 );

    m_OriginalTrainTrackData [0][488].Init ( -7373, -11931, 741, 27076, 3 );

    m_OriginalTrainTrackData [0][489].Init ( -7504, -11959, 724, 27126, 0 );

    m_OriginalTrainTrackData [0][490].Init ( -7645, -11980, 703, 27180, 101 );

    m_OriginalTrainTrackData [0][491].Init ( -7793, -11993, 678, 27235, 20 );

    m_OriginalTrainTrackData [0][492].Init ( -7949, -12003, 650, 27294, 1 );

    m_OriginalTrainTrackData [0][493].Init ( -8107, -12012, 620, 27353, 0 );

    m_OriginalTrainTrackData [0][494].Init ( -8269, -12022, 586, 27414, 9 );

    m_OriginalTrainTrackData [0][495].Init ( -8431, -12036, 551, 27475, 9 );

    m_OriginalTrainTrackData [0][496].Init ( -8592, -12056, 513, 27536, 10 );

    m_OriginalTrainTrackData [0][497].Init ( -8751, -12085, 475, 27597, 0 );

    m_OriginalTrainTrackData [0][498].Init ( -8904, -12114, 435, 27655, 225 );

    m_OriginalTrainTrackData [0][499].Init ( -9058, -12133, 397, 27713, 3 );

    m_OriginalTrainTrackData [0][500].Init ( -9212, -12144, 360, 27771, 18 );

    m_OriginalTrainTrackData [0][501].Init ( -9367, -12144, 326, 27829, 0 );

    m_OriginalTrainTrackData [0][502].Init ( -9524, -12142, 293, 27888, 137 );

    m_OriginalTrainTrackData [0][503].Init ( -9684, -12135, 264, 27948, 110 );

    m_OriginalTrainTrackData [0][504].Init ( -9848, -12124, 238, 28010, 155 );

    m_OriginalTrainTrackData [0][505].Init ( -10017, -12113, 216, 28073, 0 );

    m_OriginalTrainTrackData [0][506].Init ( -10190, -12098, 199, 28138, 75 );

    m_OriginalTrainTrackData [0][507].Init ( -10371, -12087, 187, 28206, 121 );

    m_OriginalTrainTrackData [0][508].Init ( -10529, -12084, 181, 28266, 135 );

    m_OriginalTrainTrackData [0][509].Init ( -10752, -12079, 181, 28349, 0 );

    m_OriginalTrainTrackData [0][510].Init ( -10941, -12076, 180, 28420, 82 );

    m_OriginalTrainTrackData [0][511].Init ( -11137, -12073, 176, 28494, 145 );

    m_OriginalTrainTrackData [0][512].Init ( -11340, -12069, 171, 28570, 147 );

    m_OriginalTrainTrackData [0][513].Init ( -11548, -12063, 164, 28648, 0 );

    m_OriginalTrainTrackData [0][514].Init ( -11759, -12052, 155, 28727, 109 );

    m_OriginalTrainTrackData [0][515].Init ( -11972, -12038, 147, 28807, 104 );

    m_OriginalTrainTrackData [0][516].Init ( -12187, -12018, 138, 28888, 66 );

    m_OriginalTrainTrackData [0][517].Init ( -12401, -11992, 129, 28969, 0 );

    m_OriginalTrainTrackData [0][518].Init ( -12613, -11958, 121, 29050, 64 );

    m_OriginalTrainTrackData [0][519].Init ( -12821, -11915, 114, 29129, 191 );

    m_OriginalTrainTrackData [0][520].Init ( -13026, -11862, 108, 29209, 200 );

    m_OriginalTrainTrackData [0][521].Init ( -13222, -11798, 104, 29286, 0 );

    m_OriginalTrainTrackData [0][522].Init ( -13410, -11723, 103, 29362, 155 );

    m_OriginalTrainTrackData [0][523].Init ( -13593, -11627, 103, 29439, 75 );

    m_OriginalTrainTrackData [0][524].Init ( -13773, -11507, 103, 29520, 136 );

    m_OriginalTrainTrackData [0][525].Init ( -13950, -11364, 103, 29606, 0 );

    m_OriginalTrainTrackData [0][526].Init ( -14122, -11205, 103, 29694, 145 );

    m_OriginalTrainTrackData [0][527].Init ( -14289, -11030, 103, 29784, 145 );

    m_OriginalTrainTrackData [0][528].Init ( -14450, -10844, 103, 29877, 154 );

    m_OriginalTrainTrackData [0][529].Init ( -14603, -10652, 103, 29969, 0 );

    m_OriginalTrainTrackData [0][530].Init ( -14749, -10454, 103, 30061, 117 );

    m_OriginalTrainTrackData [0][531].Init ( -14885, -10258, 103, 30150, 117 );

    m_OriginalTrainTrackData [0][532].Init ( -15013, -10064, 103, 30238, 72 );

    m_OriginalTrainTrackData [0][533].Init ( -15129, -9877, 103, 30320, 0 );

    m_OriginalTrainTrackData [0][534].Init ( -15235, -9700, 103, 30397, 74 );

    m_OriginalTrainTrackData [0][535].Init ( -15329, -9537, 103, 30468, 63 );

    m_OriginalTrainTrackData [0][536].Init ( -15412, -9384, 105, 30533, 191 );

    m_OriginalTrainTrackData [0][537].Init ( -15487, -9233, 109, 30596, 0 );

    m_OriginalTrainTrackData [0][538].Init ( -15553, -9084, 116, 30658, 113 );

    m_OriginalTrainTrackData [0][539].Init ( -15611, -8933, 124, 30718, 116 );

    m_OriginalTrainTrackData [0][540].Init ( -15663, -8782, 134, 30778, 117 );

    m_OriginalTrainTrackData [0][541].Init ( -15707, -8628, 145, 30838, 0 );

    m_OriginalTrainTrackData [0][542].Init ( -15743, -8470, 156, 30899, 111 );

    m_OriginalTrainTrackData [0][543].Init ( -15773, -8308, 166, 30961, 111 );

    m_OriginalTrainTrackData [0][544].Init ( -15797, -8140, 176, 31024, 21 );

    m_OriginalTrainTrackData [0][545].Init ( -15814, -7963, 185, 31091, 0 );

    m_OriginalTrainTrackData [0][546].Init ( -15825, -7781, 191, 31159, 23 );

    m_OriginalTrainTrackData [0][547].Init ( -15830, -7587, 196, 31232, 22 );

    m_OriginalTrainTrackData [0][548].Init ( -15831, -7382, 197, 31309, 22 );

    m_OriginalTrainTrackData [0][549].Init ( -15829, -7173, 197, 31387, 0 );

    m_OriginalTrainTrackData [0][550].Init ( -15828, -6964, 197, 31466, 201 );

    m_OriginalTrainTrackData [0][551].Init ( -15827, -6758, 197, 31543, 202 );

    m_OriginalTrainTrackData [0][552].Init ( -15827, -6552, 197, 31620, 212 );

    m_OriginalTrainTrackData [0][553].Init ( -15827, -6347, 197, 31697, 0 );

    m_OriginalTrainTrackData [0][554].Init ( -15827, -6144, 197, 31773, 208 );

    m_OriginalTrainTrackData [0][555].Init ( -15829, -5941, 197, 31849, 209 );

    m_OriginalTrainTrackData [0][556].Init ( -15830, -5739, 197, 31925, 72 );

    m_OriginalTrainTrackData [0][557].Init ( -15832, -5538, 197, 32001, 0 );

    m_OriginalTrainTrackData [0][558].Init ( -15834, -5337, 197, 32076, 117 );

    m_OriginalTrainTrackData [0][559].Init ( -15836, -5136, 197, 32151, 105 );

    m_OriginalTrainTrackData [0][560].Init ( -15838, -4936, 197, 32226, 106 );

    m_OriginalTrainTrackData [0][561].Init ( -15840, -4736, 199, 32301, 0 );

    m_OriginalTrainTrackData [0][562].Init ( -15841, -4688, 199, 32319, 126 );

    m_OriginalTrainTrackData [0][563].Init ( -15840, -4640, 199, 32337, 103 );

    m_OriginalTrainTrackData [0][564].Init ( -15841, -4592, 199, 32355, 154 );

    m_OriginalTrainTrackData [0][565].Init ( -15841, -4544, 197, 32373, 0 );

    m_OriginalTrainTrackData [0][566].Init ( -15840, -4416, 197, 32421, 133 );

    m_OriginalTrainTrackData [0][567].Init ( -15840, -4288, 197, 32469, 135 );

    m_OriginalTrainTrackData [0][568].Init ( -15839, -4160, 197, 32517, 82 );

    m_OriginalTrainTrackData [0][569].Init ( -15835, -4032, 197, 32565, 0 );

    m_OriginalTrainTrackData [0][570].Init ( -15826, -3904, 197, 32613, 149 );

    m_OriginalTrainTrackData [0][571].Init ( -15816, -3776, 197, 32662, 203 );

    m_OriginalTrainTrackData [0][572].Init ( -15803, -3648, 197, 32710, 215 );

    m_OriginalTrainTrackData [0][573].Init ( -15788, -3520, 197, 32758, 0 );

    m_OriginalTrainTrackData [0][574].Init ( -15774, -3392, 197, 32806, 78 );

    m_OriginalTrainTrackData [0][575].Init ( -15760, -3264, 197, 32855, 79 );

    m_OriginalTrainTrackData [0][576].Init ( -15748, -3136, 197, 32903, 79 );

    m_OriginalTrainTrackData [0][577].Init ( -15737, -3008, 197, 32951, 0 );

    m_OriginalTrainTrackData [0][578].Init ( -15729, -2880, 197, 32999, 199 );

    m_OriginalTrainTrackData [0][579].Init ( -15722, -2752, 197, 33047, 78 );

    m_OriginalTrainTrackData [0][580].Init ( -15712, -2624, 197, 33095, 65 );

    m_OriginalTrainTrackData [0][581].Init ( -15698, -2496, 197, 33144, 0 );

    m_OriginalTrainTrackData [0][582].Init ( -15680, -2368, 197, 33192, 78 );

    m_OriginalTrainTrackData [0][583].Init ( -15661, -2240, 197, 33241, 72 );

    m_OriginalTrainTrackData [0][584].Init ( -15641, -2112, 197, 33289, 199 );

    m_OriginalTrainTrackData [0][585].Init ( -15620, -1984, 197, 33338, 0 );

    m_OriginalTrainTrackData [0][586].Init ( -15604, -1856, 197, 33386, 105 );

    m_OriginalTrainTrackData [0][587].Init ( -15590, -1728, 197, 33435, 107 );

    m_OriginalTrainTrackData [0][588].Init ( -15578, -1600, 197, 33483, 116 );

    m_OriginalTrainTrackData [0][589].Init ( -15567, -1472, 197, 33531, 0 );

    m_OriginalTrainTrackData [0][590].Init ( -15558, -1344, 197, 33579, 207 );

    m_OriginalTrainTrackData [0][591].Init ( -15555, -1216, 197, 33627, 216 );

    m_OriginalTrainTrackData [0][592].Init ( -15555, -1088, 197, 33675, 76 );

    m_OriginalTrainTrackData [0][593].Init ( -15556, -320, 197, 33963, 0 );

    m_OriginalTrainTrackData [0][594].Init ( -15555, -95, 197, 34048, 196 );

    m_OriginalTrainTrackData [0][595].Init ( -15555, 991, 197, 34455, 77 );

    m_OriginalTrainTrackData [0][596].Init ( -15552, 1119, 197, 34503, 211 );

    m_OriginalTrainTrackData [0][597].Init ( -15548, 1247, 197, 34551, 0 );

    m_OriginalTrainTrackData [0][598].Init ( -15541, 1375, 197, 34599, 77 );

    m_OriginalTrainTrackData [0][599].Init ( -15533, 1502, 197, 34647, 76 );

    m_OriginalTrainTrackData [0][600].Init ( -15530, 1533, 197, 34658, 133 );

    m_OriginalTrainTrackData [0][601].Init ( -15516, 1660, 196, 34706, 0 );

    m_OriginalTrainTrackData [0][602].Init ( -15506, 1728, 192, 34732, 81 );

    m_OriginalTrainTrackData [0][603].Init ( -15485, 1854, 186, 34780, 127 );

    m_OriginalTrainTrackData [0][604].Init ( -15454, 1991, 174, 34833, 132 );

    m_OriginalTrainTrackData [0][605].Init ( -15415, 2107, 161, 34878, 0 );

    m_OriginalTrainTrackData [0][606].Init ( -15369, 2203, 147, 34918, 149 );

    m_OriginalTrainTrackData [0][607].Init ( -15300, 2291, 132, 34960, 149 );

    m_OriginalTrainTrackData [0][608].Init ( -15226, 2371, 117, 35001, 207 );

    m_OriginalTrainTrackData [0][609].Init ( -15149, 2441, 102, 35040, 0 );

    m_OriginalTrainTrackData [0][610].Init ( -15068, 2499, 88, 35078, 109 );

    m_OriginalTrainTrackData [0][611].Init ( -14981, 2554, 73, 35116, 120 );

    m_OriginalTrainTrackData [0][612].Init ( -14889, 2615, 57, 35158, 119 );

    m_OriginalTrainTrackData [0][613].Init ( -14788, 2683, 38, 35203, 0 );

    m_OriginalTrainTrackData [0][614].Init ( -14574, 2825, 2, 35300, 122 );

    m_OriginalTrainTrackData [0][615].Init ( -14469, 2897, -4, 35347, 106 );

    m_OriginalTrainTrackData [0][616].Init ( -14363, 2971, -3, 35396, 126 );

    m_OriginalTrainTrackData [0][617].Init ( -14258, 3045, 1, 35444, 0 );

    m_OriginalTrainTrackData [0][618].Init ( -14153, 3119, 8, 35492, 76 );

    m_OriginalTrainTrackData [0][619].Init ( -14049, 3194, 17, 35540, 77 );

    m_OriginalTrainTrackData [0][620].Init ( -13946, 3269, 30, 35588, 67 );

    m_OriginalTrainTrackData [0][621].Init ( -13842, 3344, 44, 35636, 0 );

    m_OriginalTrainTrackData [0][622].Init ( -13739, 3420, 59, 35684, 208 );

    m_OriginalTrainTrackData [0][623].Init ( -13635, 3495, 77, 35732, 125 );

    m_OriginalTrainTrackData [0][624].Init ( -13531, 3570, 95, 35780, 121 );

    m_OriginalTrainTrackData [0][625].Init ( -13428, 3645, 114, 35828, 0 );

    m_OriginalTrainTrackData [0][626].Init ( -13324, 3721, 133, 35876, 126 );

    m_OriginalTrainTrackData [0][627].Init ( -13221, 3796, 153, 35924, 129 );

    m_OriginalTrainTrackData [0][628].Init ( -13117, 3871, 171, 35972, 130 );

    m_OriginalTrainTrackData [0][629].Init ( -12910, 4022, 205, 36068, 0 );

    m_OriginalTrainTrackData [0][630].Init ( -12806, 4097, 223, 36116, 59 );

    m_OriginalTrainTrackData [0][631].Init ( -12599, 4247, 252, 36212, 195 );

    m_OriginalTrainTrackData [0][632].Init ( -12392, 4397, 267, 36308, 195 );

    m_OriginalTrainTrackData [0][633].Init ( -12184, 4548, 268, 36404, 0 );

    m_OriginalTrainTrackData [0][634].Init ( -11667, 4924, 268, 36644, 207 );

    m_OriginalTrainTrackData [0][635].Init ( -10321, 5902, 268, 37268, 128 );

    m_OriginalTrainTrackData [0][636].Init ( -8663, 7106, 268, 38036, 130 );

    m_OriginalTrainTrackData [0][637].Init ( -8043, 7557, 268, 38324, 0 );

    m_OriginalTrainTrackData [0][638].Init ( -7939, 7632, 268, 38372, 70 );

    m_OriginalTrainTrackData [0][639].Init ( -6653, 8569, 268, 38969, 126 );

    m_OriginalTrainTrackData [0][640].Init ( -6179, 8911, 260, 39188, 131 );

    m_OriginalTrainTrackData [0][641].Init ( -6085, 8979, 256, 39231, 0 );

    m_OriginalTrainTrackData [0][642].Init ( -5982, 9049, 251, 39278, 139 );

    m_OriginalTrainTrackData [0][643].Init ( -5872, 9121, 246, 39327, 148 );

    m_OriginalTrainTrackData [0][644].Init ( -5758, 9190, 241, 39377, 149 );

    m_OriginalTrainTrackData [0][645].Init ( -5609, 9271, 233, 39441, 0 );

    m_OriginalTrainTrackData [0][646].Init ( -5523, 9313, 229, 39477, 214 );

    m_OriginalTrainTrackData [0][647].Init ( -5404, 9361, 223, 39525, 100 );

    m_OriginalTrainTrackData [0][648].Init ( -5286, 9398, 218, 39571, 61 );

    m_OriginalTrainTrackData [0][649].Init ( -5139, 9429, 213, 39628, 0 );

    m_OriginalTrainTrackData [0][650].Init ( -4784, 9505, 207, 39764, 196 );

    m_OriginalTrainTrackData [0][651].Init ( -4510, 9564, 209, 39869, 69 );

    m_OriginalTrainTrackData [0][652].Init ( -3818, 9710, 227, 40134, 198 );

    m_OriginalTrainTrackData [0][653].Init ( -3500, 9782, 234, 40256, 0 );

    m_OriginalTrainTrackData [0][654].Init ( -3348, 9815, 236, 40315, 205 );

    m_OriginalTrainTrackData [0][655].Init ( -3196, 9848, 238, 40373, 128 );

    m_OriginalTrainTrackData [0][656].Init ( -3047, 9880, 238, 40430, 130 );

    m_OriginalTrainTrackData [0][657].Init ( -2941, 9903, 237, 40471, 0 );

    m_OriginalTrainTrackData [0][658].Init ( -2862, 9923, 236, 40501, 106 );

    m_OriginalTrainTrackData [0][659].Init ( -2607, 9991, 232, 40600, 59 );

    m_OriginalTrainTrackData [0][660].Init ( -2412, 10042, 228, 40676, 121 );

    m_OriginalTrainTrackData [0][661].Init ( -2305, 10070, 225, 40718, 0 );

    m_OriginalTrainTrackData [0][662].Init ( -2188, 10097, 222, 40763, 153 );

    m_OriginalTrainTrackData [0][663].Init ( -2062, 10124, 218, 40811, 86 );

    m_OriginalTrainTrackData [0][664].Init ( -1925, 10152, 212, 40863, 61 );

    m_OriginalTrainTrackData [0][665].Init ( -1775, 10179, 203, 40920, 0 );

    m_OriginalTrainTrackData [0][666].Init ( -1614, 10206, 191, 40982, 127 );

    m_OriginalTrainTrackData [0][667].Init ( -1438, 10233, 177, 41048, 124 );

    m_OriginalTrainTrackData [0][668].Init ( -1248, 10258, 164, 41120, 130 );

    m_OriginalTrainTrackData [0][669].Init ( -1042, 10282, 152, 41198, 0 );

    m_OriginalTrainTrackData [0][670].Init ( -875, 10300, 146, 41261, 143 );

    m_OriginalTrainTrackData [0][671].Init ( -718, 10315, 142, 41320, 143 );

    m_OriginalTrainTrackData [0][672].Init ( -567, 10328, 140, 41377, 56 );

    m_OriginalTrainTrackData [0][673].Init ( -423, 10337, 138, 41431, 0 );

    m_OriginalTrainTrackData [0][674].Init ( -286, 10345, 136, 41483, 218 );

    m_OriginalTrainTrackData [0][675].Init ( -153, 10348, 136, 41532, 219 );

    m_OriginalTrainTrackData [0][676].Init ( -26, 10349, 136, 41580, 218 );

    m_OriginalTrainTrackData [0][677].Init ( 96, 10347, 136, 41626, 0 );

    m_OriginalTrainTrackData [0][678].Init ( 216, 10342, 137, 41671, 207 );

    m_OriginalTrainTrackData [0][679].Init ( 332, 10333, 139, 41715, 207 );

    m_OriginalTrainTrackData [0][680].Init ( 446, 10321, 143, 41758, 207 );

    m_OriginalTrainTrackData [0][681].Init ( 557, 10305, 147, 41800, 0 );

    m_OriginalTrainTrackData [0][682].Init ( 667, 10285, 152, 41842, 220 );

    m_OriginalTrainTrackData [0][683].Init ( 776, 10261, 157, 41883, 219 );

    m_OriginalTrainTrackData [0][684].Init ( 885, 10234, 162, 41925, 240 );

    m_OriginalTrainTrackData [0][685].Init ( 993, 10201, 167, 41968, 0 );

    m_OriginalTrainTrackData [0][686].Init ( 1102, 10165, 170, 42011, 220 );

    m_OriginalTrainTrackData [0][687].Init ( 1212, 10125, 173, 42055, 162 );

    m_OriginalTrainTrackData [0][688].Init ( 1324, 10079, 174, 42100, 165 );

    m_OriginalTrainTrackData [0][689].Init ( 1438, 10029, 174, 42147, 0 );

    m_OriginalTrainTrackData [0][690].Init ( 1558, 9975, 174, 42196, 54 );

    m_OriginalTrainTrackData [0][691].Init ( 1681, 9925, 174, 42246, 121 );

    m_OriginalTrainTrackData [0][692].Init ( 1805, 9877, 174, 42296, 57 );

    m_OriginalTrainTrackData [0][693].Init ( 1930, 9834, 174, 42345, 0 );

    m_OriginalTrainTrackData [0][694].Init ( 2057, 9794, 174, 42395, 86 );

    m_OriginalTrainTrackData [0][695].Init ( 2184, 9758, 174, 42445, 70 );

    m_OriginalTrainTrackData [0][696].Init ( 2312, 9727, 174, 42494, 90 );

    m_OriginalTrainTrackData [0][697].Init ( 2440, 9700, 173, 42543, 0 );

    m_OriginalTrainTrackData [0][698].Init ( 2568, 9678, 172, 42592, 129 );

    m_OriginalTrainTrackData [0][699].Init ( 2696, 9660, 169, 42640, 127 );

    m_OriginalTrainTrackData [0][700].Init ( 2824, 9648, 165, 42689, 80 );

    m_OriginalTrainTrackData [0][701].Init ( 2951, 9640, 159, 42736, 0 );

    m_OriginalTrainTrackData [0][702].Init ( 3077, 9638, 153, 42784, 152 );

    m_OriginalTrainTrackData [0][703].Init ( 3202, 9641, 147, 42831, 137 );

    m_OriginalTrainTrackData [0][704].Init ( 3326, 9650, 140, 42877, 133 );

    m_OriginalTrainTrackData [0][705].Init ( 3449, 9665, 132, 42924, 0 );

    m_OriginalTrainTrackData [0][706].Init ( 3570, 9687, 125, 42970, 163 );

    m_OriginalTrainTrackData [0][707].Init ( 3689, 9714, 118, 43016, 138 );

    m_OriginalTrainTrackData [0][708].Init ( 3806, 9748, 112, 43061, 137 );

    m_OriginalTrainTrackData [0][709].Init ( 3920, 9789, 105, 43107, 0 );

    m_OriginalTrainTrackData [0][710].Init ( 4032, 9837, 100, 43152, 85 );

    m_OriginalTrainTrackData [0][711].Init ( 4141, 9893, 96, 43198, 89 );

    m_OriginalTrainTrackData [0][712].Init ( 4289, 9977, 91, 43262, 54 );

    m_OriginalTrainTrackData [0][713].Init ( 4447, 10067, 89, 43330, 0 );

    m_OriginalTrainTrackData [0][714].Init ( 4592, 10150, 87, 43393, 56 );

    m_OriginalTrainTrackData [0][715].Init ( 4723, 10225, 86, 43450, 95 );

    m_OriginalTrainTrackData [0][716].Init ( 4842, 10294, 86, 43501, 97 );

    m_OriginalTrainTrackData [0][717].Init ( 4952, 10358, 86, 43549, 0 );

    m_OriginalTrainTrackData [0][718].Init ( 5053, 10418, 86, 43593, 129 );

    m_OriginalTrainTrackData [0][719].Init ( 5146, 10475, 86, 43634, 92 );

    m_OriginalTrainTrackData [0][720].Init ( 5232, 10531, 86, 43672, 123 );

    m_OriginalTrainTrackData [0][721].Init ( 5314, 10586, 86, 43709, 0 );

    m_OriginalTrainTrackData [0][722].Init ( 5392, 10642, 86, 43745, 163 );

    m_OriginalTrainTrackData [0][723].Init ( 5467, 10700, 86, 43781, 164 );

    m_OriginalTrainTrackData [0][724].Init ( 5542, 10762, 86, 43817, 166 );

    m_OriginalTrainTrackData [0][725].Init ( 5613, 10823, 86, 43852, 0 );

    m_OriginalTrainTrackData [0][726].Init ( 5668, 10884, 86, 43883, 134 );

    m_OriginalTrainTrackData [0][727].Init ( 5717, 10955, 85, 43916, 35 );

    m_OriginalTrainTrackData [0][728].Init ( 5760, 11034, 85, 43949, 40 );

    m_OriginalTrainTrackData [0][729].Init ( 5797, 11123, 85, 43986, 0 );

    m_OriginalTrainTrackData [0][730].Init ( 5829, 11220, 84, 44024, 25 );

    m_OriginalTrainTrackData [0][731].Init ( 5856, 11326, 83, 44065, 51 );

    m_OriginalTrainTrackData [0][732].Init ( 5879, 11440, 82, 44108, 28 );

    m_OriginalTrainTrackData [0][733].Init ( 5897, 11561, 80, 44154, 0 );

    m_OriginalTrainTrackData [0][734].Init ( 5912, 11690, 79, 44203, 75 );

    m_OriginalTrainTrackData [0][735].Init ( 5924, 11826, 76, 44254, 91 );

    m_OriginalTrainTrackData [0][736].Init ( 5933, 11970, 74, 44308, 49 );

    m_OriginalTrainTrackData [0][737].Init ( 5939, 12120, 71, 44365, 0 );

    m_OriginalTrainTrackData [0][738].Init ( 5944, 12276, 68, 44423, 74 );

    m_OriginalTrainTrackData [0][739].Init ( 5946, 12439, 65, 44484, 74 );

    m_OriginalTrainTrackData [0][740].Init ( 5947, 12783, 57, 44613, 152 );

    m_OriginalTrainTrackData [0][741].Init ( 5947, 12924, 55, 44666, 0 );

    m_OriginalTrainTrackData [0][742].Init ( 5943, 13580, 43, 44912, 156 );

    m_OriginalTrainTrackData [0][743].Init ( 5940, 14298, 35, 45181, 157 );

    m_OriginalTrainTrackData [0][744].Init ( 5925, 16211, 45, 45899, 151 );

    m_OriginalTrainTrackData [0][745].Init ( 5914, 16910, 90, 46161, 0 );

    m_OriginalTrainTrackData [0][746].Init ( 5913, 17033, 97, 46207, 30 );

    m_OriginalTrainTrackData [0][747].Init ( 5914, 17149, 104, 46251, 63 );

    m_OriginalTrainTrackData [0][748].Init ( 5914, 17257, 110, 46291, 34 );

    m_OriginalTrainTrackData [0][749].Init ( 5911, 17357, 116, 46329, 0 );

    m_OriginalTrainTrackData [0][750].Init ( 5907, 17448, 120, 46363, 71 );

    m_OriginalTrainTrackData [0][751].Init ( 5904, 17530, 125, 46394, 22 );

    m_OriginalTrainTrackData [0][752].Init ( 5903, 17600, 128, 46420, 88 );

    m_OriginalTrainTrackData [0][753].Init ( 5902, 17662, 130, 46443, 0 );

    m_OriginalTrainTrackData [0][754].Init ( 5900, 17713, 131, 46462, 73 );

    m_OriginalTrainTrackData [0][755].Init ( 5895, 17788, 132, 46490, 34 );

    m_OriginalTrainTrackData [0][756].Init ( 5889, 17879, 134, 46525, 25 );

    m_OriginalTrainTrackData [0][757].Init ( 5878, 17978, 136, 46562, 0 );

    m_OriginalTrainTrackData [0][758].Init ( 5864, 18079, 138, 46600, 105 );

    m_OriginalTrainTrackData [0][759].Init ( 5850, 18194, 139, 46644, 137 );

    m_OriginalTrainTrackData [0][760].Init ( 5835, 18314, 141, 46689, 105 );

    m_OriginalTrainTrackData [0][761].Init ( 5822, 18439, 142, 46736, 0 );

    m_OriginalTrainTrackData [0][762].Init ( 5814, 18568, 144, 46785, 117 );

    m_OriginalTrainTrackData [0][763].Init ( 5808, 18701, 145, 46834, 66 );

    m_OriginalTrainTrackData [0][764].Init ( 5804, 18836, 146, 46885, 121 );

    m_OriginalTrainTrackData [0][765].Init ( 5804, 18973, 147, 46937, 0 );

    m_OriginalTrainTrackData [0][766].Init ( 5808, 19112, 149, 46989, 108 );

    m_OriginalTrainTrackData [0][767].Init ( 5818, 19250, 150, 47041, 136 );

    m_OriginalTrainTrackData [0][768].Init ( 5834, 19388, 151, 47093, 141 );

    m_OriginalTrainTrackData [0][769].Init ( 5855, 19525, 153, 47145, 0 );

    m_OriginalTrainTrackData [0][770].Init ( 5882, 19659, 154, 47196, 112 );

    m_OriginalTrainTrackData [0][771].Init ( 5918, 19791, 155, 47247, 166 );

    m_OriginalTrainTrackData [0][772].Init ( 5967, 19918, 156, 47298, 164 );

    m_OriginalTrainTrackData [0][773].Init ( 6030, 20063, 157, 47358, 0 );

    m_OriginalTrainTrackData [0][774].Init ( 6173, 20385, 158, 47490, 89 );

    m_OriginalTrainTrackData [0][775].Init ( 6237, 20525, 158, 47547, 92 );

    m_OriginalTrainTrackData [0][776].Init ( 6298, 20652, 158, 47600, 98 );

    m_OriginalTrainTrackData [0][777].Init ( 6355, 20768, 158, 47649, 0 );

    m_OriginalTrainTrackData [0][778].Init ( 6409, 20875, 158, 47694, 101 );

    m_OriginalTrainTrackData [0][779].Init ( 6459, 20974, 158, 47735, 88 );

    m_OriginalTrainTrackData [0][780].Init ( 6507, 21066, 158, 47774, 146 );

    m_OriginalTrainTrackData [0][781].Init ( 6552, 21152, 158, 47811, 0 );

    m_OriginalTrainTrackData [0][782].Init ( 6606, 21258, 158, 47855, 91 );

    m_OriginalTrainTrackData [0][783].Init ( 6633, 21308, 158, 47876, 151 );

    m_OriginalTrainTrackData [0][784].Init ( 6690, 21408, 158, 47920, 188 );

    m_OriginalTrainTrackData [0][785].Init ( 6756, 21495, 158, 47961, 0 );

    m_OriginalTrainTrackData [0][786].Init ( 6842, 21598, 158, 48011, 100 );

    m_OriginalTrainTrackData [0][787].Init ( 6928, 21697, 157, 48060, 74 );

    m_OriginalTrainTrackData [0][788].Init ( 7018, 21788, 155, 48108, 179 );

    m_OriginalTrainTrackData [0][789].Init ( 7112, 21871, 153, 48155, 0 );

    m_OriginalTrainTrackData [0][790].Init ( 7211, 21942, 151, 48201, 181 );

    m_OriginalTrainTrackData [0][791].Init ( 7316, 21999, 148, 48246, 99 );

    m_OriginalTrainTrackData [0][792].Init ( 7430, 22041, 146, 48291, 183 );

    m_OriginalTrainTrackData [0][793].Init ( 7548, 22064, 140, 48336, 0 );

    m_OriginalTrainTrackData [0][794].Init ( 7661, 22069, 136, 48379, 185 );

    m_OriginalTrainTrackData [0][795].Init ( 7738, 22068, 133, 48407, 184 );

    m_OriginalTrainTrackData [0][796].Init ( 7822, 22066, 130, 48439, 184 );

    m_OriginalTrainTrackData [0][797].Init ( 7916, 22065, 126, 48474, 0 );

    m_OriginalTrainTrackData [0][798].Init ( 7983, 22063, 122, 48499, 99 );

    m_OriginalTrainTrackData [0][799].Init ( 8065, 22050, 120, 48531, 187 );

    m_OriginalTrainTrackData [0][800].Init ( 8149, 22028, 117, 48563, 187 );

    m_OriginalTrainTrackData [0][801].Init ( 8234, 21995, 115, 48597, 0 );

    m_OriginalTrainTrackData [0][802].Init ( 8318, 21953, 113, 48632, 184 );

    m_OriginalTrainTrackData [0][803].Init ( 8404, 21903, 111, 48670, 181 );

    m_OriginalTrainTrackData [0][804].Init ( 8490, 21848, 109, 48708, 189 );

    m_OriginalTrainTrackData [0][805].Init ( 8577, 21780, 107, 48749, 0 );

    m_OriginalTrainTrackData [0][806].Init ( 8664, 21707, 105, 48792, 188 );

    m_OriginalTrainTrackData [0][807].Init ( 8751, 21636, 103, 48834, 178 );

    m_OriginalTrainTrackData [0][808].Init ( 8839, 21571, 101, 48875, 190 );

    m_OriginalTrainTrackData [0][809].Init ( 8926, 21502, 99, 48917, 0 );

    m_OriginalTrainTrackData [0][810].Init ( 9013, 21432, 97, 48959, 151 );

    m_OriginalTrainTrackData [0][811].Init ( 9099, 21363, 94, 49000, 102 );

    m_OriginalTrainTrackData [0][812].Init ( 9186, 21298, 92, 49041, 99 );

    m_OriginalTrainTrackData [0][813].Init ( 9271, 21239, 90, 49080, 0 );

    m_OriginalTrainTrackData [0][814].Init ( 9357, 21186, 88, 49117, 96 );

    m_OriginalTrainTrackData [0][815].Init ( 9442, 21147, 86, 49153, 93 );

    m_OriginalTrainTrackData [0][816].Init ( 9526, 21112, 84, 49187, 93 );

    m_OriginalTrainTrackData [0][817].Init ( 9608, 21085, 82, 49219, 0 );

    m_OriginalTrainTrackData [0][818].Init ( 9693, 21067, 80, 49252, 84 );

    m_OriginalTrainTrackData [0][819].Init ( 9775, 21058, 78, 49283, 89 );

    m_OriginalTrainTrackData [0][820].Init ( 9888, 21058, 78, 49325, 91 );

    m_OriginalTrainTrackData [0][821].Init ( 11196, 21058, 78, 49815, 0 );

    m_OriginalTrainTrackData [0][822].Init ( 11516, 21058, 78, 49935, 45 );

    m_OriginalTrainTrackData [0][823].Init ( 11716, 21058, 78, 50010, 45 );

    m_OriginalTrainTrackData [0][824].Init ( 11855, 21058, 78, 50063, 45 );

    m_OriginalTrainTrackData [0][825].Init ( 12785, 21058, 78, 50411, 0 );

    m_OriginalTrainTrackData [0][826].Init ( 12847, 21058, 78, 50435, 41 );

    m_OriginalTrainTrackData [0][827].Init ( 12863, 21059, 78, 50441, 43 );

    m_OriginalTrainTrackData [0][828].Init ( 12879, 21059, 78, 50447, 41 );

    m_OriginalTrainTrackData [0][829].Init ( 12895, 21062, 78, 50453, 0 );

    m_OriginalTrainTrackData [0][830].Init ( 12911, 21067, 78, 50459, 45 );

    m_OriginalTrainTrackData [0][831].Init ( 12926, 21072, 78, 50465, 45 );

    m_OriginalTrainTrackData [0][832].Init ( 12943, 21077, 78, 50472, 81 );

    m_OriginalTrainTrackData [0][833].Init ( 12958, 21082, 78, 50477, 0 );

    m_OriginalTrainTrackData [0][834].Init ( 12974, 21085, 78, 50484, 82 );

    m_OriginalTrainTrackData [0][835].Init ( 12991, 21087, 78, 50490, 45 );

    m_OriginalTrainTrackData [0][836].Init ( 13007, 21090, 78, 50496, 47 );

    m_OriginalTrainTrackData [0][837].Init ( 13039, 21089, 78, 50508, 0 );

    m_OriginalTrainTrackData [0][838].Init ( 13122, 21093, 78, 50539, 46 );

    m_OriginalTrainTrackData [0][839].Init ( 13206, 21101, 78, 50571, 81 );

    m_OriginalTrainTrackData [0][840].Init ( 13293, 21115, 78, 50604, 77 );

    m_OriginalTrainTrackData [0][841].Init ( 13381, 21133, 78, 50638, 0 );

    m_OriginalTrainTrackData [0][842].Init ( 13470, 21155, 78, 50672, 77 );

    m_OriginalTrainTrackData [0][843].Init ( 13560, 21179, 78, 50707, 85 );

    m_OriginalTrainTrackData [0][844].Init ( 13651, 21208, 78, 50743, 84 );

    m_OriginalTrainTrackData [0][845].Init ( 13742, 21237, 78, 50779, 0 );

    m_OriginalTrainTrackData [0][846].Init ( 13833, 21269, 78, 50815, 78 );

    m_OriginalTrainTrackData [0][847].Init ( 14016, 21335, 78, 50888, 83 );

    m_OriginalTrainTrackData [0][848].Init ( 14108, 21368, 78, 50924, 86 );

    m_OriginalTrainTrackData [0][849].Init ( 14198, 21400, 78, 50960, 0 );

    m_OriginalTrainTrackData [0][850].Init ( 14289, 21430, 78, 50996, 48 );

    m_OriginalTrainTrackData [0][851].Init ( 14379, 21459, 78, 51032, 97 );

    m_OriginalTrainTrackData [0][852].Init ( 14467, 21484, 78, 51066, 82 );

    m_OriginalTrainTrackData [0][853].Init ( 14555, 21508, 78, 51100, 0 );

    m_OriginalTrainTrackData [0][854].Init ( 14642, 21527, 78, 51133, 74 );

    m_OriginalTrainTrackData [0][855].Init ( 14728, 21541, 78, 51166, 189 );

    m_OriginalTrainTrackData [0][856].Init ( 14812, 21550, 78, 51198, 74 );

    m_OriginalTrainTrackData [0][857].Init ( 14893, 21553, 78, 51228, 0 );

    m_OriginalTrainTrackData [0][858].Init ( 15535, 21553, 78, 51469, 39 );

    m_OriginalTrainTrackData [0][859].Init ( 16175, 21553, 78, 51709, 42 );

    m_OriginalTrainTrackData [0][860].Init ( 16739, 21553, 80, 51920, 43 );

    m_OriginalTrainTrackData [0][861].Init ( 17199, 21553, 78, 52093, 0 );

    m_OriginalTrainTrackData [0][862].Init ( 17231, 21551, 78, 52105, 53 );

    m_OriginalTrainTrackData [0][863].Init ( 17247, 21549, 78, 52111, 28 );

    m_OriginalTrainTrackData [0][864].Init ( 17263, 21547, 78, 52117, 39 );

    m_OriginalTrainTrackData [0][865].Init ( 17279, 21545, 78, 52123, 0 );

    m_OriginalTrainTrackData [0][866].Init ( 17295, 21540, 78, 52129, 94 );

    m_OriginalTrainTrackData [0][867].Init ( 17311, 21535, 78, 52136, 94 );

    m_OriginalTrainTrackData [0][868].Init ( 17327, 21530, 78, 52142, 27 );

    m_OriginalTrainTrackData [0][869].Init ( 17343, 21526, 78, 52148, 0 );

    m_OriginalTrainTrackData [0][870].Init ( 17359, 21523, 78, 52154, 75 );

    m_OriginalTrainTrackData [0][871].Init ( 17375, 21522, 78, 52160, 75 );

    m_OriginalTrainTrackData [0][872].Init ( 17391, 21521, 78, 52166, 154 );

    m_OriginalTrainTrackData [0][873].Init ( 17455, 21521, 78, 52190, 0 );

    m_OriginalTrainTrackData [0][874].Init ( 17837, 21521, 78, 52334, 60 );

    m_OriginalTrainTrackData [0][875].Init ( 19375, 21521, 78, 52910, 32 );

    m_OriginalTrainTrackData [0][876].Init ( 19503, 21521, 78, 52958, 63 );

    m_OriginalTrainTrackData [0][877].Init ( 19588, 21516, 78, 52990, 0 );

    m_OriginalTrainTrackData [0][878].Init ( 19669, 21500, 78, 53021, 23 );

    m_OriginalTrainTrackData [0][879].Init ( 19746, 21476, 78, 53051, 70 );

    m_OriginalTrainTrackData [0][880].Init ( 19818, 21443, 78, 53081, 55 );

    m_OriginalTrainTrackData [0][881].Init ( 19886, 21403, 78, 53111, 0 );

    m_OriginalTrainTrackData [0][882].Init ( 19948, 21356, 78, 53140, 58 );

    m_OriginalTrainTrackData [0][883].Init ( 20007, 21302, 78, 53170, 28 );

    m_OriginalTrainTrackData [0][884].Init ( 20061, 21244, 78, 53200, 90 );

    m_OriginalTrainTrackData [0][885].Init ( 20110, 21182, 78, 53229, 0 );

    m_OriginalTrainTrackData [0][886].Init ( 20156, 21115, 78, 53260, 157 );

    m_OriginalTrainTrackData [0][887].Init ( 20197, 21046, 78, 53290, 151 );

    m_OriginalTrainTrackData [0][888].Init ( 20235, 20975, 78, 53320, 156 );

    m_OriginalTrainTrackData [0][889].Init ( 20268, 20902, 78, 53350, 0 );

    m_OriginalTrainTrackData [0][890].Init ( 20296, 20829, 78, 53379, 157 );

    m_OriginalTrainTrackData [0][891].Init ( 20322, 20755, 78, 53409, 175 );

    m_OriginalTrainTrackData [0][892].Init ( 20342, 20683, 78, 53437, 158 );

    m_OriginalTrainTrackData [0][893].Init ( 20359, 20611, 78, 53465, 0 );

    m_OriginalTrainTrackData [0][894].Init ( 20373, 20543, 78, 53491, 158 );

    m_OriginalTrainTrackData [0][895].Init ( 20382, 20476, 78, 53516, 171 );

    m_OriginalTrainTrackData [0][896].Init ( 20388, 20414, 78, 53539, 171 );

    m_OriginalTrainTrackData [0][897].Init ( 20390, 20355, 78, 53561, 0 );

    m_OriginalTrainTrackData [0][898].Init ( 20390, 20228, 78, 53609, 47 );

    m_OriginalTrainTrackData [0][899].Init ( 20390, 20196, 78, 53621, 47 );

    m_OriginalTrainTrackData [0][900].Init ( 20390, 20100, 78, 53657, 36 );

    m_OriginalTrainTrackData [0][901].Init ( 20389, 19971, 78, 53705, 0 );

    m_OriginalTrainTrackData [0][902].Init ( 20390, 19843, 78, 53753, 48 );

    m_OriginalTrainTrackData [0][903].Init ( 20390, 19827, 78, 53759, 53 );

    m_OriginalTrainTrackData [0][904].Init ( 20391, 19811, 78, 53765, 35 );

    m_OriginalTrainTrackData [0][905].Init ( 20394, 19795, 78, 53772, 0 );

    m_OriginalTrainTrackData [0][906].Init ( 20399, 19779, 78, 53778, 175 );

    m_OriginalTrainTrackData [0][907].Init ( 20404, 19763, 78, 53784, 172 );

    m_OriginalTrainTrackData [0][908].Init ( 20409, 19748, 78, 53790, 177 );

    m_OriginalTrainTrackData [0][909].Init ( 20413, 19732, 78, 53796, 0 );

    m_OriginalTrainTrackData [0][910].Init ( 20416, 19715, 78, 53803, 46 );

    m_OriginalTrainTrackData [0][911].Init ( 20418, 19699, 78, 53809, 47 );

    m_OriginalTrainTrackData [0][912].Init ( 20420, 19683, 78, 53815, 53 );

    m_OriginalTrainTrackData [0][913].Init ( 20421, 19667, 78, 53821, 0 );

    m_OriginalTrainTrackData [0][914].Init ( 20422, 19634, 78, 53833, 106 );

    m_OriginalTrainTrackData [0][915].Init ( 20422, 19526, 77, 53874, 105 );

    m_OriginalTrainTrackData [0][916].Init ( 20421, 19397, 74, 53922, 106 );

    m_OriginalTrainTrackData [0][917].Init ( 20421, 19268, 70, 53970, 0 );

    m_OriginalTrainTrackData [0][918].Init ( 20421, 19137, 63, 54020, 109 );

    m_OriginalTrainTrackData [0][919].Init ( 20421, 19017, 56, 54065, 109 );

    m_OriginalTrainTrackData [0][920].Init ( 20421, 18884, 45, 54114, 109 );

    m_OriginalTrainTrackData [0][921].Init ( 20422, 18759, 33, 54161, 0 );

    m_OriginalTrainTrackData [0][922].Init ( 20422, 18632, 20, 54209, 118 );

    m_OriginalTrainTrackData [0][923].Init ( 20422, 18594, 19, 54223, 116 );

    m_OriginalTrainTrackData [0][924].Init ( 20423, 18550, 19, 54240, 120 );

    m_OriginalTrainTrackData [0][925].Init ( 20428, 18460, 18, 54274, 0 );

    m_OriginalTrainTrackData [0][0].Init ( 20443, 18338, 18, 0, 0 );

    m_OriginalTrainTrackData [0][1].Init ( 20472, 18224, 15, 44, 0 );

    m_OriginalTrainTrackData [0][2].Init ( 20508, 18117, 12, 86, 0 );

    m_OriginalTrainTrackData [0][3].Init ( 20555, 18020, 9, 126, 0 );

    m_OriginalTrainTrackData [0][4].Init ( 20608, 17929, 4, 166, 97 );

    m_OriginalTrainTrackData [0][5].Init ( 20670, 17848, 0, 204, 96 );

    m_OriginalTrainTrackData [0][6].Init ( 20736, 17770, -4, 242, 163 );

    m_OriginalTrainTrackData [0][7].Init ( 20812, 17698, -9, 282, 97 );

    m_OriginalTrainTrackData [0][8].Init ( 20888, 17633, -13, 319, 160 );

    m_OriginalTrainTrackData [0][9].Init ( 20970, 17568, -17, 358, 137 );

    m_OriginalTrainTrackData [0][10].Init ( 21056, 17508, -21, 398, 137 );

    m_OriginalTrainTrackData [0][11].Init ( 21142, 17450, -24, 437, 96 );

    m_OriginalTrainTrackData [0][12].Init ( 21229, 17392, -26, 476, 94 );

    m_OriginalTrainTrackData [0][13].Init ( 21316, 17336, -28, 515, 97 );

    m_OriginalTrainTrackData [0][14].Init ( 21408, 17275, -28, 556, 96 );

    m_OriginalTrainTrackData [0][15].Init ( 21491, 17220, -27, 593, 94 );

    m_OriginalTrainTrackData [0][16].Init ( 21574, 17160, -25, 632, 13 );

    m_OriginalTrainTrackData [0][17].Init ( 21655, 17100, -22, 670, 128 );

    m_OriginalTrainTrackData [0][18].Init ( 21733, 17035, -19, 708, 219 );

    m_OriginalTrainTrackData [0][19].Init ( 21808, 16967, -15, 746, 13 );

    m_OriginalTrainTrackData [0][20].Init ( 21880, 16893, -10, 784, 96 );

    m_OriginalTrainTrackData [0][21].Init ( 21949, 16814, -5, 824, 219 );

    m_OriginalTrainTrackData [0][22].Init ( 22011, 16728, 0, 863, 13 );

    m_OriginalTrainTrackData [0][23].Init ( 22070, 16635, 5, 905, 64 );

    m_OriginalTrainTrackData [0][24].Init ( 22121, 16535, 10, 947, 219 );

    m_OriginalTrainTrackData [0][25].Init ( 22167, 16428, 14, 991, 40 );

    m_OriginalTrainTrackData [0][26].Init ( 22206, 16309, 19, 1037, 254 );

    m_OriginalTrainTrackData [0][27].Init ( 22236, 16184, 22, 1086, 179 );

    m_OriginalTrainTrackData [0][28].Init ( 22240, 16162, 23, 1094, 13 );

    m_OriginalTrainTrackData [0][29].Init ( 22261, 16046, 25, 1138, 232 );

    m_OriginalTrainTrackData [0][30].Init ( 22265, 16000, 26, 1156, 169 );

    m_OriginalTrainTrackData [0][31].Init ( 22274, 15895, 27, 1195, 13 );

    m_OriginalTrainTrackData [0][32].Init ( 22277, 15819, 28, 1224, 222 );

    m_OriginalTrainTrackData [0][33].Init ( 22279, 15758, 28, 1247, 13 );

    m_OriginalTrainTrackData [0][34].Init ( 22279, 15719, 28, 1261, 13 );

    m_OriginalTrainTrackData [0][35].Init ( 22278, 15695, 28, 1270, 254 );

    m_OriginalTrainTrackData [0][36].Init ( 22276, 15672, 28, 1279, 13 );

    m_OriginalTrainTrackData [0][37].Init ( 22268, 15642, 28, 1290, 1 );

    m_OriginalTrainTrackData [0][38].Init ( 22261, 15620, 28, 1299, 96 );

    m_OriginalTrainTrackData [0][39].Init ( 22252, 15591, 28, 1310, 219 );

    m_OriginalTrainTrackData [0][40].Init ( 22248, 15565, 28, 1320, 1 );

    m_OriginalTrainTrackData [0][41].Init ( 22248, 15548, 28, 1327, 128 );

    m_OriginalTrainTrackData [0][42].Init ( 22248, 15419, 28, 1375, 219 );

    m_OriginalTrainTrackData [0][43].Init ( 22248, 15318, 33, 1413, 148 );

    m_OriginalTrainTrackData [0][44].Init ( 22248, 15307, 34, 1417, 32 );

    m_OriginalTrainTrackData [0][45].Init ( 22248, 15193, 43, 1460, 219 );

    m_OriginalTrainTrackData [0][46].Init ( 22248, 15088, 55, 1499, 1 );

    m_OriginalTrainTrackData [0][47].Init ( 22248, 14986, 65, 1537, 64 );

    m_OriginalTrainTrackData [0][48].Init ( 22248, 14919, 71, 1563, 219 );

    m_OriginalTrainTrackData [0][49].Init ( 22248, 14826, 76, 1597, 1 );

    m_OriginalTrainTrackData [0][50].Init ( 22248, 14756, 78, 1624, 96 );

    m_OriginalTrainTrackData [0][51].Init ( 22248, 14679, 78, 1653, 219 );

    m_OriginalTrainTrackData [0][52].Init ( 22248, 14601, 78, 1682, 1 );

    m_OriginalTrainTrackData [0][53].Init ( 22248, 14529, 78, 1709, 96 );

    m_OriginalTrainTrackData [0][54].Init ( 22248, 14470, 78, 1731, 219 );

    m_OriginalTrainTrackData [0][55].Init ( 22247, 14392, 78, 1760, 1 );

    m_OriginalTrainTrackData [0][56].Init ( 22247, 14309, 78, 1791, 128 );

    m_OriginalTrainTrackData [0][57].Init ( 22247, 14218, 78, 1825, 219 );

    m_OriginalTrainTrackData [0][58].Init ( 22247, 14148, 78, 1852, 1 );

    m_OriginalTrainTrackData [0][59].Init ( 22247, 14065, 78, 1883, 96 );

    m_OriginalTrainTrackData [0][60].Init ( 22247, 13950, 78, 1926, 219 );

    m_OriginalTrainTrackData [0][61].Init ( 22247, 13878, 78, 1953, 238 );

    m_OriginalTrainTrackData [0][62].Init ( 22247, 13825, 78, 1973, 64 );

    m_OriginalTrainTrackData [0][63].Init ( 22247, 13780, 78, 1990, 219 );

    m_OriginalTrainTrackData [0][64].Init ( 22247, 13746, 78, 2002, 238 );

    m_OriginalTrainTrackData [0][65].Init ( 22247, 13728, 78, 2009, 64 );

    m_OriginalTrainTrackData [0][66].Init ( 22247, 13690, 78, 2023, 219 );

    m_OriginalTrainTrackData [0][67].Init ( 22247, 13658, 78, 2035, 1 );

    m_OriginalTrainTrackData [0][68].Init ( 22247, 13628, 78, 2047, 128 );

    m_OriginalTrainTrackData [0][69].Init ( 22247, 13600, 78, 2057, 219 );

    m_OriginalTrainTrackData [0][70].Init ( 22248, 13570, 78, 2068, 1 );

    m_OriginalTrainTrackData [0][71].Init ( 22248, 13526, 78, 2085, 96 );

    m_OriginalTrainTrackData [0][72].Init ( 22248, 13485, 78, 2100, 219 );

    m_OriginalTrainTrackData [0][73].Init ( 22248, 13440, 78, 2117, 1 );

    m_OriginalTrainTrackData [0][74].Init ( 22248, 13398, 78, 2133, 128 );

    m_OriginalTrainTrackData [0][75].Init ( 22248, 13360, 78, 2147, 219 );

    m_OriginalTrainTrackData [0][76].Init ( 22248, 13330, 78, 2158, 1 );

    m_OriginalTrainTrackData [0][77].Init ( 22248, 13289, 78, 2174, 96 );

    m_OriginalTrainTrackData [0][78].Init ( 22248, 13269, 78, 2181, 219 );

    m_OriginalTrainTrackData [0][79].Init ( 22249, 13235, 78, 2194, 1 );

    m_OriginalTrainTrackData [0][80].Init ( 22250, 13201, 78, 2207, 96 );

    m_OriginalTrainTrackData [0][81].Init ( 22254, 13171, 78, 2218, 219 );

    m_OriginalTrainTrackData [0][82].Init ( 22261, 13142, 78, 2229, 1 );

    m_OriginalTrainTrackData [0][83].Init ( 22277, 13114, 78, 2242, 96 );

    m_OriginalTrainTrackData [0][84].Init ( 22300, 13081, 78, 2257, 219 );

    m_OriginalTrainTrackData [0][85].Init ( 22337, 13033, 78, 2279, 1 );

    m_OriginalTrainTrackData [0][86].Init ( 22395, 12967, 78, 2312, 96 );

    m_OriginalTrainTrackData [0][87].Init ( 22445, 12906, 78, 2342, 219 );

    m_OriginalTrainTrackData [0][88].Init ( 22488, 12854, 78, 2367, 1 );

    m_OriginalTrainTrackData [0][89].Init ( 22535, 12795, 78, 2395, 96 );

    m_OriginalTrainTrackData [0][90].Init ( 22589, 12737, 78, 2425, 219 );

    m_OriginalTrainTrackData [0][91].Init ( 22639, 12674, 78, 2455, 1 );

    m_OriginalTrainTrackData [0][92].Init ( 22685, 12608, 78, 2485, 96 );

    m_OriginalTrainTrackData [0][93].Init ( 22725, 12538, 78, 2516, 219 );

    m_OriginalTrainTrackData [0][94].Init ( 22763, 12467, 78, 2546, 1 );

    m_OriginalTrainTrackData [0][95].Init ( 22796, 12394, 78, 2576, 96 );

    m_OriginalTrainTrackData [0][96].Init ( 22825, 12321, 78, 2605, 219 );

    m_OriginalTrainTrackData [0][97].Init ( 22850, 12248, 78, 2634, 1 );

    m_OriginalTrainTrackData [0][98].Init ( 22870, 12176, 78, 2662, 128 );

    m_OriginalTrainTrackData [0][99].Init ( 22888, 12105, 78, 2690, 219 );

    m_OriginalTrainTrackData [0][100].Init ( 22901, 12035, 78, 2717, 230 );

    m_OriginalTrainTrackData [0][101].Init ( 22910, 11969, 78, 2742, 64 );

    m_OriginalTrainTrackData [0][102].Init ( 22915, 11907, 78, 2765, 179 );

    m_OriginalTrainTrackData [0][103].Init ( 22918, 11865, 78, 2781, 1 );

    m_OriginalTrainTrackData [0][104].Init ( 22918, 11545, 78, 2901, 254 );

    m_OriginalTrainTrackData [0][105].Init ( 22918, 10714, 78, 3212, 219 );

    m_OriginalTrainTrackData [0][106].Init ( 22918, 10201, 78, 3405, 1 );

    m_OriginalTrainTrackData [0][107].Init ( 22918, 9913, 78, 3513, 254 );

    m_OriginalTrainTrackData [0][108].Init ( 22918, 9785, 78, 3561, 2 );

    m_OriginalTrainTrackData [0][109].Init ( 22904, 9704, 77, 3591, 1 );

    m_OriginalTrainTrackData [0][110].Init ( 22877, 9613, 77, 3627, 243 );

    m_OriginalTrainTrackData [0][111].Init ( 22839, 9507, 77, 3669, 189 );

    m_OriginalTrainTrackData [0][112].Init ( 22791, 9390, 77, 3717, 238 );

    m_OriginalTrainTrackData [0][113].Init ( 22734, 9265, 78, 3768, 128 );

    m_OriginalTrainTrackData [0][114].Init ( 22669, 9133, 78, 3823, 172 );

    m_OriginalTrainTrackData [0][115].Init ( 22598, 9000, 78, 3880, 238 );

    m_OriginalTrainTrackData [0][116].Init ( 22523, 8866, 78, 3938, 64 );

    m_OriginalTrainTrackData [0][117].Init ( 22444, 8735, 78, 3995, 172 );

    m_OriginalTrainTrackData [0][118].Init ( 22255, 8480, 87, 4114, 238 );

    m_OriginalTrainTrackData [0][119].Init ( 22191, 8391, 83, 4155, 96 );

    m_OriginalTrainTrackData [0][120].Init ( 22145, 8307, 82, 4191, 182 );

    m_OriginalTrainTrackData [0][121].Init ( 22121, 8233, 79, 4220, 1 );

    m_OriginalTrainTrackData [0][122].Init ( 22118, 8186, 79, 4238, 128 );

    m_OriginalTrainTrackData [0][123].Init ( 22118, 7866, 79, 4358, 182 );

    m_OriginalTrainTrackData [0][124].Init ( 22118, 5946, 79, 5078, 148 );

    m_OriginalTrainTrackData [0][125].Init ( 22122, 4229, 61, 5722, 32 );

    m_OriginalTrainTrackData [0][126].Init ( 22123, 3782, 58, 5889, 182 );

    m_OriginalTrainTrackData [0][127].Init ( 22119, 3542, 55, 5979, 110 );

    m_OriginalTrainTrackData [0][128].Init ( 22119, 3259, 53, 6085, 96 );

    m_OriginalTrainTrackData [0][129].Init ( 22123, 3030, 53, 6171, 182 );

    m_OriginalTrainTrackData [0][130].Init ( 22122, 2795, 53, 6259, 97 );

    m_OriginalTrainTrackData [0][131].Init ( 22121, 2586, 55, 6338, 160 );

    m_OriginalTrainTrackData [0][132].Init ( 22122, 2362, 58, 6422, 182 );

    m_OriginalTrainTrackData [0][133].Init ( 22138, 2253, 58, 6463, 1 );

    m_OriginalTrainTrackData [0][134].Init ( 22168, 2141, 63, 6507, 96 );

    m_OriginalTrainTrackData [0][135].Init ( 22250, 1916, 68, 6596, 182 );

    m_OriginalTrainTrackData [0][136].Init ( 22344, 1668, 83, 6696, 1 );

    m_OriginalTrainTrackData [0][137].Init ( 22412, 1448, 109, 6782, 96 );

    m_OriginalTrainTrackData [0][138].Init ( 22445, 1342, 124, 6824, 182 );

    m_OriginalTrainTrackData [0][139].Init ( 22484, 1229, 139, 6869, 1 );

    m_OriginalTrainTrackData [0][140].Init ( 22510, 1134, 153, 6906, 96 );

    m_OriginalTrainTrackData [0][141].Init ( 22535, 1026, 168, 6947, 182 );

    m_OriginalTrainTrackData [0][142].Init ( 22565, 842, 192, 7017, 1 );

    m_OriginalTrainTrackData [0][143].Init ( 22587, 682, 203, 7078, 32 );

    m_OriginalTrainTrackData [0][144].Init ( 22607, 513, 208, 7141, 182 );

    m_OriginalTrainTrackData [0][145].Init ( 22620, 290, 215, 7225, 1 );

    m_OriginalTrainTrackData [0][146].Init ( 22624, -643, 262, 7575, 96 );

    m_OriginalTrainTrackData [0][147].Init ( 22626, -741, 263, 7612, 73 );

    m_OriginalTrainTrackData [0][148].Init ( 22622, -834, 265, 7647, 1 );

    m_OriginalTrainTrackData [0][149].Init ( 22615, -896, 263, 7670, 32 );

    m_OriginalTrainTrackData [0][150].Init ( 22601, -961, 261, 7695, 73 );

    m_OriginalTrainTrackData [0][151].Init ( 22578, -1037, 256, 7725, 1 );

    m_OriginalTrainTrackData [0][152].Init ( 22542, -1129, 249, 7762, 160 );

    m_OriginalTrainTrackData [0][153].Init ( 22501, -1225, 241, 7801, 86 );

    m_OriginalTrainTrackData [0][154].Init ( 22455, -1333, 232, 7845, 110 );

    m_OriginalTrainTrackData [0][155].Init ( 22397, -1450, 220, 7894, 128 );

    m_OriginalTrainTrackData [0][156].Init ( 22344, -1557, 209, 7939, 182 );

    m_OriginalTrainTrackData [0][157].Init ( 22283, -1667, 197, 7986, 1 );

    m_OriginalTrainTrackData [0][158].Init ( 22218, -1786, 184, 8037, 173 );

    m_OriginalTrainTrackData [0][159].Init ( 22155, -1906, 170, 8088, 182 );

    m_OriginalTrainTrackData [0][160].Init ( 22104, -2001, 159, 8128, 148 );

    m_OriginalTrainTrackData [0][161].Init ( 22071, -2061, 152, 8154, 96 );

    m_OriginalTrainTrackData [0][162].Init ( 22042, -2108, 146, 8175, 182 );

    m_OriginalTrainTrackData [0][163].Init ( 22002, -2157, 141, 8198, 161 );

    m_OriginalTrainTrackData [0][164].Init ( 21955, -2196, 136, 8221, 160 );

    m_OriginalTrainTrackData [0][165].Init ( 21902, -2223, 132, 8243, 182 );

    m_OriginalTrainTrackData [0][166].Init ( 21842, -2244, 129, 8267, 1 );

    m_OriginalTrainTrackData [0][167].Init ( 21764, -2271, 124, 8298, 64 );

    m_OriginalTrainTrackData [0][168].Init ( 21562, -2321, 115, 8376, 182 );

    m_OriginalTrainTrackData [0][169].Init ( 21378, -2349, 107, 8446, 1 );

    m_OriginalTrainTrackData [0][170].Init ( 21193, -2362, 102, 8516, 96 );

    m_OriginalTrainTrackData [0][171].Init ( 20998, -2358, 100, 8589, 182 );

    m_OriginalTrainTrackData [0][172].Init ( 20804, -2342, 103, 8662, 1 );

    m_OriginalTrainTrackData [0][173].Init ( 20668, -2325, 108, 8713, 128 );

    m_OriginalTrainTrackData [0][174].Init ( 20533, -2301, 113, 8765, 219 );

    m_OriginalTrainTrackData [0][175].Init ( 20397, -2280, 117, 8816, 13 );

    m_OriginalTrainTrackData [0][176].Init ( 20263, -2262, 122, 8867, 96 );

    m_OriginalTrainTrackData [0][177].Init ( 20136, -2245, 125, 8915, 219 );

    m_OriginalTrainTrackData [0][178].Init ( 20062, -2236, 128, 8943, 13 );

    m_OriginalTrainTrackData [0][179].Init ( 19998, -2227, 129, 8967, 128 );

    m_OriginalTrainTrackData [0][180].Init ( 19740, -2199, 135, 9064, 219 );

    m_OriginalTrainTrackData [0][181].Init ( 19622, -2191, 137, 9109, 32 );

    m_OriginalTrainTrackData [0][182].Init ( 19492, -2182, 139, 9158, 96 );

    m_OriginalTrainTrackData [0][183].Init ( 19378, -2174, 140, 9200, 219 );

    m_OriginalTrainTrackData [0][184].Init ( 19255, -2175, 140, 9247, 13 );

    m_OriginalTrainTrackData [0][185].Init ( 19146, -2183, 146, 9288, 128 );

    m_OriginalTrainTrackData [0][186].Init ( 19031, -2206, 151, 9332, 219 );

    m_OriginalTrainTrackData [0][187].Init ( 18925, -2245, 166, 9374, 32 );

    m_OriginalTrainTrackData [0][188].Init ( 18826, -2287, 179, 9414, 32 );

    m_OriginalTrainTrackData [0][189].Init ( 18728, -2340, 197, 9456, 219 );

    m_OriginalTrainTrackData [0][190].Init ( 18633, -2400, 216, 9498, 32 );

    m_OriginalTrainTrackData [0][191].Init ( 18542, -2462, 235, 9539, 32 );

    m_OriginalTrainTrackData [0][192].Init ( 18446, -2526, 255, 9583, 219 );

    m_OriginalTrainTrackData [0][193].Init ( 18356, -2587, 273, 9624, 13 );

    m_OriginalTrainTrackData [0][194].Init ( 18261, -2649, 290, 9666, 128 );

    m_OriginalTrainTrackData [0][195].Init ( 18172, -2700, 300, 9705, 219 );

    m_OriginalTrainTrackData [0][196].Init ( 18071, -2750, 312, 9747, 13 );

    m_OriginalTrainTrackData [0][197].Init ( 17983, -2784, 320, 9782, 96 );

    m_OriginalTrainTrackData [0][198].Init ( 17883, -2816, 330, 9822, 219 );

    m_OriginalTrainTrackData [0][199].Init ( 17800, -2835, 342, 9853, 13 );

    m_OriginalTrainTrackData [0][200].Init ( 17707, -2851, 354, 9889, 96 );

    m_OriginalTrainTrackData [0][201].Init ( 17539, -2863, 381, 9952, 219 );

    m_OriginalTrainTrackData [0][202].Init ( 17377, -2862, 408, 10013, 32 );

    m_OriginalTrainTrackData [0][203].Init ( 17218, -2854, 431, 10072, 96 );

    m_OriginalTrainTrackData [0][204].Init ( 17057, -2850, 448, 10133, 81 );

    m_OriginalTrainTrackData [0][205].Init ( 16883, -2859, 465, 10198, 32 );

    m_OriginalTrainTrackData [0][206].Init ( 16787, -2871, 475, 10234, 173 );

    m_OriginalTrainTrackData [0][207].Init ( 16694, -2891, 485, 10270, 219 );

    m_OriginalTrainTrackData [0][208].Init ( 16599, -2918, 496, 10307, 32 );

    m_OriginalTrainTrackData [0][209].Init ( 16503, -2953, 507, 10346, 160 );

    m_OriginalTrainTrackData [0][210].Init ( 16412, -2999, 517, 10384, 219 );

    m_OriginalTrainTrackData [0][211].Init ( 16324, -3051, 527, 10422, 13 );

    m_OriginalTrainTrackData [0][212].Init ( 16244, -3121, 534, 10462, 96 );

    m_OriginalTrainTrackData [0][213].Init ( 16171, -3194, 541, 10501, 219 );

    m_OriginalTrainTrackData [0][214].Init ( 16109, -3288, 544, 10543, 32 );

    m_OriginalTrainTrackData [0][215].Init ( 16057, -3387, 547, 10585, 96 );

    m_OriginalTrainTrackData [0][216].Init ( 16015, -3491, 549, 10627, 219 );

    m_OriginalTrainTrackData [0][217].Init ( 15984, -3602, 551, 10670, 238 );

    m_OriginalTrainTrackData [0][218].Init ( 15962, -3698, 555, 10707, 96 );

    m_OriginalTrainTrackData [0][219].Init ( 15948, -3814, 559, 10751, 182 );

    m_OriginalTrainTrackData [0][220].Init ( 15945, -3915, 563, 10789, 1 );

    m_OriginalTrainTrackData [0][221].Init ( 15957, -4023, 569, 10829, 254 );

    m_OriginalTrainTrackData [0][222].Init ( 15982, -4131, 571, 10871, 246 );

    m_OriginalTrainTrackData [0][223].Init ( 16018, -4231, 573, 10911, 1 );

    m_OriginalTrainTrackData [0][224].Init ( 16077, -4336, 571, 10956, 64 );

    m_OriginalTrainTrackData [0][225].Init ( 16143, -4437, 568, 11001, 219 );

    m_OriginalTrainTrackData [0][226].Init ( 16237, -4546, 562, 11055, 41 );

    m_OriginalTrainTrackData [0][227].Init ( 16336, -4643, 555, 11107, 232 );

    m_OriginalTrainTrackData [0][228].Init ( 16456, -4748, 541, 11167, 219 );

    m_OriginalTrainTrackData [0][229].Init ( 16560, -4840, 529, 11219, 1 );

    m_OriginalTrainTrackData [0][230].Init ( 16670, -4932, 511, 11273, 222 );

    m_OriginalTrainTrackData [0][231].Init ( 16767, -5013, 496, 11320, 246 );

    m_OriginalTrainTrackData [0][232].Init ( 16872, -5097, 477, 11371, 1 );

    m_OriginalTrainTrackData [0][233].Init ( 16976, -5180, 458, 11421, 232 );

    m_OriginalTrainTrackData [0][234].Init ( 17087, -5266, 438, 11473, 219 );

    m_OriginalTrainTrackData [0][235].Init ( 17194, -5348, 416, 11524, 217 );

    m_OriginalTrainTrackData [0][236].Init ( 17342, -5459, 392, 11593, 222 );

    m_OriginalTrainTrackData [0][237].Init ( 17429, -5525, 375, 11634, 86 );

    m_OriginalTrainTrackData [0][238].Init ( 17480, -5564, 368, 11658, 57 );

    m_OriginalTrainTrackData [0][239].Init ( 17690, -5720, 339, 11756, 160 );

    m_OriginalTrainTrackData [0][240].Init ( 17790, -5813, 324, 11808, 123 );

    m_OriginalTrainTrackData [0][241].Init ( 17922, -5934, 306, 11875, 57 );

    m_OriginalTrainTrackData [0][242].Init ( 18081, -6163, 278, 11979, 96 );

    m_OriginalTrainTrackData [0][243].Init ( 18182, -6405, 256, 12078, 22 );

    m_OriginalTrainTrackData [0][244].Init ( 18239, -6658, 235, 12175, 233 );

    m_OriginalTrainTrackData [0][245].Init ( 18267, -6918, 220, 12273, 98 );

    m_OriginalTrainTrackData [0][246].Init ( 18279, -7182, 204, 12372, 160 );

    m_OriginalTrainTrackData [0][247].Init ( 18279, -7422, 205, 12462, 140 );

    m_OriginalTrainTrackData [0][248].Init ( 18279, -7682, 206, 12559, 100 );

    m_OriginalTrainTrackData [0][249].Init ( 18279, -7940, 207, 12656, 99 );

    m_OriginalTrainTrackData [0][250].Init ( 18279, -8182, 208, 12747, 204 );

    m_OriginalTrainTrackData [0][251].Init ( 18279, -8422, 208, 12837, 97 );

    m_OriginalTrainTrackData [0][252].Init ( 18279, -8682, 208, 12934, 220 );

    m_OriginalTrainTrackData [0][253].Init ( 18279, -9100, 206, 13091, 144 );

    m_OriginalTrainTrackData [0][254].Init ( 18279, -9292, 205, 13163, 98 );

    m_OriginalTrainTrackData [0][255].Init ( 18279, -9488, 198, 13237, 218 );

    m_OriginalTrainTrackData [0][256].Init ( 18279, -9744, 185, 13333, 172 );

    m_OriginalTrainTrackData [0][257].Init ( 18279, -10256, 185, 13525, 105 );

    m_OriginalTrainTrackData [0][258].Init ( 18279, -10384, 185, 13573, 224 );

    m_OriginalTrainTrackData [0][259].Init ( 18279, -10512, 185, 13621, 159 );

    m_OriginalTrainTrackData [0][260].Init ( 18279, -10640, 185, 13669, 2 );

    m_OriginalTrainTrackData [0][261].Init ( 18279, -10768, 185, 13717, 67 );

    m_OriginalTrainTrackData [0][262].Init ( 18279, -10896, 185, 13765, 202 );

    m_OriginalTrainTrackData [0][263].Init ( 18279, -10976, 185, 13795, 160 );

    m_OriginalTrainTrackData [0][264].Init ( 18278, -11032, 184, 13816, 16 );

    m_OriginalTrainTrackData [0][265].Init ( 18279, -11072, 184, 13831, 68 );

    m_OriginalTrainTrackData [0][266].Init ( 18279, -11112, 184, 13846, 10 );

    m_OriginalTrainTrackData [0][267].Init ( 18279, -11168, 184, 13867, 60 );

    m_OriginalTrainTrackData [0][268].Init ( 18275, -11295, 184, 13914, 192 );

    m_OriginalTrainTrackData [0][269].Init ( 18267, -11423, 183, 13962, 97 );

    m_OriginalTrainTrackData [0][270].Init ( 18252, -11549, 181, 14010, 16 );

    m_OriginalTrainTrackData [0][271].Init ( 18231, -11675, 178, 14058, 189 );

    m_OriginalTrainTrackData [0][272].Init ( 18205, -11792, 175, 14103, 94 );

    m_OriginalTrainTrackData [0][273].Init ( 18194, -11832, 174, 14118, 159 );

    m_OriginalTrainTrackData [0][274].Init ( 18185, -11872, 172, 14134, 1 );

    m_OriginalTrainTrackData [0][275].Init ( 18175, -11912, 172, 14149, 67 );

    m_OriginalTrainTrackData [0][276].Init ( 18163, -11952, 171, 14165, 146 );

    m_OriginalTrainTrackData [0][277].Init ( 18132, -12042, 166, 14201, 48 );

    m_OriginalTrainTrackData [0][278].Init ( 18086, -12159, 160, 14248, 208 );

    m_OriginalTrainTrackData [0][279].Init ( 18048, -12247, 154, 14284, 254 );

    m_OriginalTrainTrackData [0][280].Init ( 18002, -12361, 151, 14330, 201 );

    m_OriginalTrainTrackData [0][281].Init ( 17954, -12479, 145, 14378, 54 );

    m_OriginalTrainTrackData [0][282].Init ( 17856, -12716, 133, 14474, 12 );

    m_OriginalTrainTrackData [0][283].Init ( 17810, -12829, 126, 14520, 10 );

    m_OriginalTrainTrackData [0][284].Init ( 17759, -12946, 121, 14567, 9 );

    m_OriginalTrainTrackData [0][285].Init ( 17713, -13079, 116, 14620, 191 );

    m_OriginalTrainTrackData [0][286].Init ( 17701, -13125, 115, 14638, 65 );

    m_OriginalTrainTrackData [0][287].Init ( 17689, -13171, 114, 14656, 159 );

    m_OriginalTrainTrackData [0][288].Init ( 17676, -13218, 113, 14674, 2 );

    m_OriginalTrainTrackData [0][289].Init ( 17663, -13264, 111, 14692, 245 );

    m_OriginalTrainTrackData [0][290].Init ( 17634, -13373, 108, 14734, 248 );

    m_OriginalTrainTrackData [0][291].Init ( 17613, -13486, 105, 14777, 48 );

    m_OriginalTrainTrackData [0][292].Init ( 17595, -13621, 101, 14829, 0 );

    m_OriginalTrainTrackData [0][293].Init ( 17589, -13762, 99, 14881, 128 );

    m_OriginalTrainTrackData [0][294].Init ( 17588, -13810, 99, 14899, 144 );

    m_OriginalTrainTrackData [0][295].Init ( 17588, -13858, 99, 14917, 66 );

    m_OriginalTrainTrackData [0][296].Init ( 17588, -13906, 99, 14935, 129 );

    m_OriginalTrainTrackData [0][297].Init ( 17589, -13954, 99, 14953, 202 );

    m_OriginalTrainTrackData [0][298].Init ( 17589, -14082, 99, 15001, 4 );

    m_OriginalTrainTrackData [0][299].Init ( 17589, -14898, 102, 15307, 189 );

    m_OriginalTrainTrackData [0][300].Init ( 17589, -14931, 102, 15320, 5 );

    m_OriginalTrainTrackData [0][301].Init ( 17589, -15059, 102, 15368, 159 );

    m_OriginalTrainTrackData [0][302].Init ( 17589, -15115, 102, 15389, 251 );

    m_OriginalTrainTrackData [0][303].Init ( 17589, -15155, 102, 15404, 33 );

    m_OriginalTrainTrackData [0][304].Init ( 17589, -15195, 102, 15419, 204 );

    m_OriginalTrainTrackData [0][305].Init ( 17590, -15251, 102, 15440, 48 );

    m_OriginalTrainTrackData [0][306].Init ( 17583, -15306, 102, 15461, 208 );

    m_OriginalTrainTrackData [0][307].Init ( 17571, -15354, 102, 15479, 254 );

    m_OriginalTrainTrackData [0][308].Init ( 17550, -15404, 102, 15500, 11 );

    m_OriginalTrainTrackData [0][309].Init ( 17526, -15447, 102, 15518, 123 );

    m_OriginalTrainTrackData [0][310].Init ( 17493, -15490, 102, 15538, 94 );

    m_OriginalTrainTrackData [0][311].Init ( 17458, -15525, 102, 15557, 46 );

    m_OriginalTrainTrackData [0][312].Init ( 17409, -15564, 102, 15580, 20 );

    m_OriginalTrainTrackData [0][313].Init ( 17366, -15589, 102, 15599, 189 );

    m_OriginalTrainTrackData [0][314].Init ( 17316, -15610, 102, 15619, 101 );

    m_OriginalTrainTrackData [0][315].Init ( 17268, -15623, 102, 15638, 159 );

    m_OriginalTrainTrackData [0][316].Init ( 17215, -15630, 102, 15658, 249 );

    m_OriginalTrainTrackData [0][317].Init ( 17166, -15630, 102, 15676, 199 );

    m_OriginalTrainTrackData [0][318].Init ( 17069, -15630, 102, 15713, 16 );

    m_OriginalTrainTrackData [0][319].Init ( 16941, -15630, 102, 15761, 48 );

    m_OriginalTrainTrackData [0][320].Init ( 16813, -15630, 102, 15809, 75 );

    m_OriginalTrainTrackData [0][321].Init ( 16557, -15630, 102, 15905, 65 );

    m_OriginalTrainTrackData [0][322].Init ( 16429, -15630, 102, 15953, 153 );

    m_OriginalTrainTrackData [0][323].Init ( 16173, -15630, 102, 16049, 144 );

    m_OriginalTrainTrackData [0][324].Init ( 15917, -15630, 102, 16145, 126 );

    m_OriginalTrainTrackData [0][325].Init ( 15653, -15630, 101, 16244, 183 );

    m_OriginalTrainTrackData [0][326].Init ( 15517, -15630, 102, 16295, 160 );

    m_OriginalTrainTrackData [0][327].Init ( 15389, -15630, 102, 16343, 170 );

    m_OriginalTrainTrackData [0][328].Init ( 15024, -15630, 102, 16480, 9 );

    m_OriginalTrainTrackData [0][329].Init ( 14733, -15631, 100, 16589, 141 );

    m_OriginalTrainTrackData [0][330].Init ( 13800, -15630, 100, 16939, 87 );

    m_OriginalTrainTrackData [0][331].Init ( 13575, -15629, 100, 17023, 65 );

    m_OriginalTrainTrackData [0][332].Init ( 11994, -15630, 100, 17616, 190 );

    m_OriginalTrainTrackData [0][333].Init ( 11875, -15630, 100, 17661, 221 );

    m_OriginalTrainTrackData [0][334].Init ( 11780, -15632, 100, 17696, 208 );

    m_OriginalTrainTrackData [0][335].Init ( 11684, -15632, 100, 17732, 127 );

    m_OriginalTrainTrackData [0][336].Init ( 11588, -15630, 100, 17768, 206 );

    m_OriginalTrainTrackData [0][337].Init ( 11491, -15626, 97, 17805, 123 );

    m_OriginalTrainTrackData [0][338].Init ( 11392, -15618, 94, 17842, 121 );

    m_OriginalTrainTrackData [0][339].Init ( 11290, -15604, 89, 17880, 57 );

    m_OriginalTrainTrackData [0][340].Init ( 11187, -15585, 81, 17920, 142 );

    m_OriginalTrainTrackData [0][341].Init ( 11080, -15558, 69, 17961, 0 );

    m_OriginalTrainTrackData [0][342].Init ( 10970, -15523, 58, 18004, 136 );

    m_OriginalTrainTrackData [0][343].Init ( 10855, -15480, 41, 18050, 132 );

    m_OriginalTrainTrackData [0][344].Init ( 10737, -15426, 20, 18099, 8 );

    m_OriginalTrainTrackData [0][345].Init ( 10675, -15390, 10, 18126, 0 );

    m_OriginalTrainTrackData [0][346].Init ( 10607, -15342, 0, 18157, 17 );

    m_OriginalTrainTrackData [0][347].Init ( 10533, -15280, -8, 18193, 16 );

    m_OriginalTrainTrackData [0][348].Init ( 10454, -15209, -16, 18233, 250 );

    m_OriginalTrainTrackData [0][349].Init ( 10372, -15128, -22, 18276, 0 );

    m_OriginalTrainTrackData [0][350].Init ( 10286, -15038, -28, 18323, 234 );

    m_OriginalTrainTrackData [0][351].Init ( 10197, -14941, -30, 18373, 10 );

    m_OriginalTrainTrackData [0][352].Init ( 10104, -14839, -32, 18424, 19 );

    m_OriginalTrainTrackData [0][353].Init ( 10011, -14732, -35, 18477, 0 );

    m_OriginalTrainTrackData [0][354].Init ( 9796, -14486, -40, 18600, 224 );

    m_OriginalTrainTrackData [0][355].Init ( 9581, -14238, -42, 18723, 222 );

    m_OriginalTrainTrackData [0][356].Init ( 9365, -13985, -43, 18848, 247 );

    m_OriginalTrainTrackData [0][357].Init ( 9147, -13732, -43, 18973, 0 );

    m_OriginalTrainTrackData [0][358].Init ( 8927, -13476, -42, 19100, 249 );

    m_OriginalTrainTrackData [0][359].Init ( 8704, -13220, -40, 19227, 250 );

    m_OriginalTrainTrackData [0][360].Init ( 8480, -12963, -37, 19355, 1 );

    m_OriginalTrainTrackData [0][361].Init ( 8252, -12706, -34, 19484, 0 );

    m_OriginalTrainTrackData [0][362].Init ( 8021, -12449, -31, 19613, 224 );

    m_OriginalTrainTrackData [0][363].Init ( 7787, -12195, -27, 19743, 2 );

    m_OriginalTrainTrackData [0][364].Init ( 7550, -11943, -25, 19872, 7 );

    m_OriginalTrainTrackData [0][365].Init ( 7309, -11693, -22, 20003, 0 );

    m_OriginalTrainTrackData [0][366].Init ( 7063, -11447, -21, 20133, 7 );

    m_OriginalTrainTrackData [0][367].Init ( 6814, -11205, -20, 20263, 3 );

    m_OriginalTrainTrackData [0][368].Init ( 6688, -11088, -21, 20328, 3 );

    m_OriginalTrainTrackData [0][369].Init ( 6559, -10967, -21, 20394, 0 );

    m_OriginalTrainTrackData [0][370].Init ( 6519, -10932, -21, 20414, 11 );

    m_OriginalTrainTrackData [0][371].Init ( 6393, -10821, -20, 20477, 247 );

    m_OriginalTrainTrackData [0][372].Init ( 6225, -10669, -20, 20562, 101 );

    m_OriginalTrainTrackData [0][373].Init ( 6203, -10647, -20, 20574, 0 );

    m_OriginalTrainTrackData [0][374].Init ( 5840, -10340, -13, 20752, 7 );

    m_OriginalTrainTrackData [0][375].Init ( 5472, -10045, -1, 20929, 0 );

    m_OriginalTrainTrackData [0][376].Init ( 5098, -9766, 14, 21104, 4 );

    m_OriginalTrainTrackData [0][377].Init ( 4716, -9502, 32, 21278, 0 );

    m_OriginalTrainTrackData [0][378].Init ( 4327, -9256, 53, 21450, 0 );

    m_OriginalTrainTrackData [0][379].Init ( 3931, -9032, 74, 21621, 3 );

    m_OriginalTrainTrackData [0][380].Init ( 3523, -8828, 95, 21792, 7 );

    m_OriginalTrainTrackData [0][381].Init ( 3108, -8647, 116, 21962, 0 );

    m_OriginalTrainTrackData [0][382].Init ( 2683, -8491, 134, 22132, 131 );

    m_OriginalTrainTrackData [0][383].Init ( 2248, -8362, 150, 22302, 136 );

    m_OriginalTrainTrackData [0][384].Init ( 1802, -8261, 161, 22473, 133 );

    m_OriginalTrainTrackData [0][385].Init ( 1345, -8190, 168, 22647, 0 );

    m_OriginalTrainTrackData [0][386].Init ( 876, -8151, 169, 22823, 248 );

    m_OriginalTrainTrackData [0][387].Init ( 718, -8148, 167, 22883, 133 );

    m_OriginalTrainTrackData [0][388].Init ( 396, -8144, 163, 23003, 131 );

    m_OriginalTrainTrackData [0][389].Init ( 302, -8144, 160, 23039, 0 );

    m_OriginalTrainTrackData [0][390].Init ( 208, -8144, 159, 23074, 11 );

    m_OriginalTrainTrackData [0][391].Init ( 114, -8144, 159, 23109, 10 );

    m_OriginalTrainTrackData [0][392].Init ( 20, -8144, 159, 23144, 247 );

    m_OriginalTrainTrackData [0][393].Init ( -72, -8143, 158, 23179, 0 );

    m_OriginalTrainTrackData [0][394].Init ( -166, -8144, 156, 23214, 132 );

    m_OriginalTrainTrackData [0][395].Init ( -260, -8145, 154, 23249, 227 );

    m_OriginalTrainTrackData [0][396].Init ( -354, -8144, 150, 23285, 227 );

    m_OriginalTrainTrackData [0][397].Init ( -448, -8144, 145, 23320, 0 );

    m_OriginalTrainTrackData [0][398].Init ( -543, -8144, 137, 23355, 231 );

    m_OriginalTrainTrackData [0][399].Init ( -637, -8144, 127, 23391, 234 );

    m_OriginalTrainTrackData [0][400].Init ( -730, -8144, 114, 23426, 235 );

    m_OriginalTrainTrackData [0][401].Init ( -853, -8149, 98, 23472, 0 );

    m_OriginalTrainTrackData [0][402].Init ( -979, -8164, 88, 23519, 238 );

    m_OriginalTrainTrackData [0][403].Init ( -1106, -8188, 82, 23568, 243 );

    m_OriginalTrainTrackData [0][404].Init ( -1234, -8220, 81, 23617, 240 );

    m_OriginalTrainTrackData [0][405].Init ( -1361, -8260, 84, 23667, 0 );

    m_OriginalTrainTrackData [0][406].Init ( -1484, -8308, 89, 23717, 239 );

    m_OriginalTrainTrackData [0][407].Init ( -1601, -8362, 98, 23765, 228 );

    m_OriginalTrainTrackData [0][408].Init ( -1711, -8423, 108, 23812, 241 );

    m_OriginalTrainTrackData [0][409].Init ( -1813, -8488, 120, 23858, 0 );

    m_OriginalTrainTrackData [0][410].Init ( -1903, -8559, 132, 23900, 246 );

    m_OriginalTrainTrackData [0][411].Init ( -1980, -8634, 146, 23941, 239 );

    m_OriginalTrainTrackData [0][412].Init ( -2042, -8713, 159, 23978, 238 );

    m_OriginalTrainTrackData [0][413].Init ( -2098, -8795, 172, 24016, 0 );

    m_OriginalTrainTrackData [0][414].Init ( -2159, -8883, 187, 24056, 248 );

    m_OriginalTrainTrackData [0][415].Init ( -2224, -8975, 202, 24098, 243 );

    m_OriginalTrainTrackData [0][416].Init ( -2291, -9070, 217, 24142, 221 );

    m_OriginalTrainTrackData [0][417].Init ( -2362, -9166, 233, 24186, 0 );

    m_OriginalTrainTrackData [0][418].Init ( -2432, -9263, 248, 24231, 144 );

    m_OriginalTrainTrackData [0][419].Init ( -2503, -9359, 263, 24276, 144 );

    m_OriginalTrainTrackData [0][420].Init ( -2573, -9451, 276, 24319, 144 );

    m_OriginalTrainTrackData [0][421].Init ( -2641, -9539, 289, 24361, 0 );

    m_OriginalTrainTrackData [0][422].Init ( -2705, -9622, 299, 24400, 149 );

    m_OriginalTrainTrackData [0][423].Init ( -2765, -9698, 308, 24437, 38 );

    m_OriginalTrainTrackData [0][424].Init ( -2820, -9765, 314, 24469, 30 );

    m_OriginalTrainTrackData [0][425].Init ( -2878, -9824, 319, 24500, 0 );

    m_OriginalTrainTrackData [0][426].Init ( -2950, -9880, 322, 24535, 66 );

    m_OriginalTrainTrackData [0][427].Init ( -3034, -9931, 325, 24571, 30 );

    m_OriginalTrainTrackData [0][428].Init ( -3129, -9976, 326, 24611, 71 );

    m_OriginalTrainTrackData [0][429].Init ( -3230, -10012, 328, 24651, 0 );

    m_OriginalTrainTrackData [0][430].Init ( -3336, -10039, 328, 24692, 90 );

    m_OriginalTrainTrackData [0][431].Init ( -3446, -10055, 329, 24734, 95 );

    m_OriginalTrainTrackData [0][432].Init ( -3555, -10060, 329, 24775, 96 );

    m_OriginalTrainTrackData [0][433].Init ( -3662, -10051, 328, 24815, 0 );

    m_OriginalTrainTrackData [0][434].Init ( -3765, -10027, 328, 24855, 87 );

    m_OriginalTrainTrackData [0][435].Init ( -3861, -9988, 328, 24893, 27 );

    m_OriginalTrainTrackData [0][436].Init ( -3948, -9932, 328, 24932, 145 );

    m_OriginalTrainTrackData [0][437].Init ( -4027, -9872, 328, 24969, 0 );

    m_OriginalTrainTrackData [0][438].Init ( -4092, -9821, 328, 25000, 146 );

    m_OriginalTrainTrackData [0][439].Init ( -4149, -9778, 328, 25027, 147 );

    m_OriginalTrainTrackData [0][440].Init ( -4199, -9740, 328, 25051, 149 );

    m_OriginalTrainTrackData [0][441].Init ( -4245, -9704, 328, 25073, 0 );

    m_OriginalTrainTrackData [0][442].Init ( -4292, -9668, 328, 25095, 37 );

    m_OriginalTrainTrackData [0][443].Init ( -4342, -9629, 328, 25119, 34 );

    m_OriginalTrainTrackData [0][444].Init ( -4398, -9584, 328, 25146, 68 );

    m_OriginalTrainTrackData [0][445].Init ( -4465, -9531, 328, 25178, 0 );

    m_OriginalTrainTrackData [0][446].Init ( -4543, -9466, 328, 25216, 69 );

    m_OriginalTrainTrackData [0][447].Init ( -4639, -9388, 328, 25262, 44 );

    m_OriginalTrainTrackData [0][448].Init ( -4752, -9293, 328, 25317, 92 );

    m_OriginalTrainTrackData [0][449].Init ( -4876, -9197, 331, 25376, 0 );

    m_OriginalTrainTrackData [0][450].Init ( -4998, -9119, 339, 25430, 29 );

    m_OriginalTrainTrackData [0][451].Init ( -5116, -9058, 351, 25480, 71 );

    m_OriginalTrainTrackData [0][452].Init ( -5231, -9012, 366, 25527, 29 );

    m_OriginalTrainTrackData [0][453].Init ( -5343, -8982, 384, 25570, 0 );

    m_OriginalTrainTrackData [0][454].Init ( -5452, -8966, 403, 25612, 254 );

    m_OriginalTrainTrackData [0][455].Init ( -5558, -8964, 422, 25651, 254 );

    m_OriginalTrainTrackData [0][456].Init ( -5659, -8974, 441, 25689, 4 );

    m_OriginalTrainTrackData [0][457].Init ( -5757, -8996, 458, 25727, 0 );

    m_OriginalTrainTrackData [0][458].Init ( -5851, -9028, 472, 25764, 12 );

    m_OriginalTrainTrackData [0][459].Init ( -5940, -9071, 482, 25801, 11 );

    m_OriginalTrainTrackData [0][460].Init ( -6026, -9122, 488, 25839, 24 );

    m_OriginalTrainTrackData [0][461].Init ( -6105, -9175, 492, 25874, 0 );

    m_OriginalTrainTrackData [0][462].Init ( -6177, -9229, 496, 25908, 14 );

    m_OriginalTrainTrackData [0][463].Init ( -6243, -9283, 501, 25940, 13 );

    m_OriginalTrainTrackData [0][464].Init ( -6303, -9341, 506, 25972, 3 );

    m_OriginalTrainTrackData [0][465].Init ( -6356, -9404, 513, 26002, 0 );

    m_OriginalTrainTrackData [0][466].Init ( -6404, -9474, 520, 26034, 255 );

    m_OriginalTrainTrackData [0][467].Init ( -6447, -9554, 530, 26068, 5 );

    m_OriginalTrainTrackData [0][468].Init ( -6485, -9646, 540, 26106, 5 );

    m_OriginalTrainTrackData [0][469].Init ( -6520, -9750, 553, 26147, 0 );

    m_OriginalTrainTrackData [0][470].Init ( -6551, -9869, 567, 26193, 223 );

    m_OriginalTrainTrackData [0][471].Init ( -6579, -10007, 584, 26246, 18 );

    m_OriginalTrainTrackData [0][472].Init ( -6605, -10163, 602, 26305, 19 );

    m_OriginalTrainTrackData [0][473].Init ( -6628, -10329, 622, 26368, 0 );

    m_OriginalTrainTrackData [0][474].Init ( -6636, -10392, 630, 26392, 17 );

    m_OriginalTrainTrackData [0][475].Init ( -6650, -10492, 642, 26430, 22 );

    m_OriginalTrainTrackData [0][476].Init ( -6670, -10652, 662, 26490, 17 );

    m_OriginalTrainTrackData [0][477].Init ( -6693, -10807, 680, 26549, 0 );

    m_OriginalTrainTrackData [0][478].Init ( -6715, -10958, 697, 26606, 14 );

    m_OriginalTrainTrackData [0][479].Init ( -6743, -11101, 713, 26661, 15 );

    m_OriginalTrainTrackData [0][480].Init ( -6776, -11237, 728, 26713, 16 );

    m_OriginalTrainTrackData [0][481].Init ( -6814, -11364, 740, 26763, 0 );

    m_OriginalTrainTrackData [0][482].Init ( -6860, -11482, 750, 26810, 20 );

    m_OriginalTrainTrackData [0][483].Init ( -6915, -11590, 758, 26856, 2 );

    m_OriginalTrainTrackData [0][484].Init ( -6981, -11686, 762, 26899, 6 );

    m_OriginalTrainTrackData [0][485].Init ( -7058, -11770, 764, 26942, 0 );

    m_OriginalTrainTrackData [0][486].Init ( -7149, -11838, 761, 26985, 2 );

    m_OriginalTrainTrackData [0][487].Init ( -7254, -11892, 754, 27029, 7 );

    m_OriginalTrainTrackData [0][488].Init ( -7373, -11931, 741, 27076, 3 );

    m_OriginalTrainTrackData [0][489].Init ( -7504, -11959, 724, 27126, 0 );

    m_OriginalTrainTrackData [0][490].Init ( -7645, -11980, 703, 27180, 101 );

    m_OriginalTrainTrackData [0][491].Init ( -7793, -11993, 678, 27235, 20 );

    m_OriginalTrainTrackData [0][492].Init ( -7949, -12003, 650, 27294, 1 );

    m_OriginalTrainTrackData [0][493].Init ( -8107, -12012, 620, 27353, 0 );

    m_OriginalTrainTrackData [0][494].Init ( -8269, -12022, 586, 27414, 9 );

    m_OriginalTrainTrackData [0][495].Init ( -8431, -12036, 551, 27475, 9 );

    m_OriginalTrainTrackData [0][496].Init ( -8592, -12056, 513, 27536, 10 );

    m_OriginalTrainTrackData [0][497].Init ( -8751, -12085, 475, 27597, 0 );

    m_OriginalTrainTrackData [0][498].Init ( -8904, -12114, 435, 27655, 225 );

    m_OriginalTrainTrackData [0][499].Init ( -9058, -12133, 397, 27713, 3 );

    m_OriginalTrainTrackData [0][500].Init ( -9212, -12144, 360, 27771, 18 );

    m_OriginalTrainTrackData [0][501].Init ( -9367, -12144, 326, 27829, 0 );

    m_OriginalTrainTrackData [0][502].Init ( -9524, -12142, 293, 27888, 137 );

    m_OriginalTrainTrackData [0][503].Init ( -9684, -12135, 264, 27948, 110 );

    m_OriginalTrainTrackData [0][504].Init ( -9848, -12124, 238, 28010, 155 );

    m_OriginalTrainTrackData [0][505].Init ( -10017, -12113, 216, 28073, 0 );

    m_OriginalTrainTrackData [0][506].Init ( -10190, -12098, 199, 28138, 75 );

    m_OriginalTrainTrackData [0][507].Init ( -10371, -12087, 187, 28206, 121 );

    m_OriginalTrainTrackData [0][508].Init ( -10529, -12084, 181, 28266, 135 );

    m_OriginalTrainTrackData [0][509].Init ( -10752, -12079, 181, 28349, 0 );

    m_OriginalTrainTrackData [0][510].Init ( -10941, -12076, 180, 28420, 82 );

    m_OriginalTrainTrackData [0][511].Init ( -11137, -12073, 176, 28494, 145 );

    m_OriginalTrainTrackData [0][512].Init ( -11340, -12069, 171, 28570, 147 );

    m_OriginalTrainTrackData [0][513].Init ( -11548, -12063, 164, 28648, 0 );

    m_OriginalTrainTrackData [0][514].Init ( -11759, -12052, 155, 28727, 109 );

    m_OriginalTrainTrackData [0][515].Init ( -11972, -12038, 147, 28807, 104 );

    m_OriginalTrainTrackData [0][516].Init ( -12187, -12018, 138, 28888, 66 );

    m_OriginalTrainTrackData [0][517].Init ( -12401, -11992, 129, 28969, 0 );

    m_OriginalTrainTrackData [0][518].Init ( -12613, -11958, 121, 29050, 64 );

    m_OriginalTrainTrackData [0][519].Init ( -12821, -11915, 114, 29129, 191 );

    m_OriginalTrainTrackData [0][520].Init ( -13026, -11862, 108, 29209, 200 );

    m_OriginalTrainTrackData [0][521].Init ( -13222, -11798, 104, 29286, 0 );

    m_OriginalTrainTrackData [0][522].Init ( -13410, -11723, 103, 29362, 155 );

    m_OriginalTrainTrackData [0][523].Init ( -13593, -11627, 103, 29439, 75 );

    m_OriginalTrainTrackData [0][524].Init ( -13773, -11507, 103, 29520, 136 );

    m_OriginalTrainTrackData [0][525].Init ( -13950, -11364, 103, 29606, 0 );

    m_OriginalTrainTrackData [0][526].Init ( -14122, -11205, 103, 29694, 145 );

    m_OriginalTrainTrackData [0][527].Init ( -14289, -11030, 103, 29784, 145 );

    m_OriginalTrainTrackData [0][528].Init ( -14450, -10844, 103, 29877, 154 );

    m_OriginalTrainTrackData [0][529].Init ( -14603, -10652, 103, 29969, 0 );

    m_OriginalTrainTrackData [0][530].Init ( -14749, -10454, 103, 30061, 117 );

    m_OriginalTrainTrackData [0][531].Init ( -14885, -10258, 103, 30150, 117 );

    m_OriginalTrainTrackData [0][532].Init ( -15013, -10064, 103, 30238, 72 );

    m_OriginalTrainTrackData [0][533].Init ( -15129, -9877, 103, 30320, 0 );

    m_OriginalTrainTrackData [0][534].Init ( -15235, -9700, 103, 30397, 74 );

    m_OriginalTrainTrackData [0][535].Init ( -15329, -9537, 103, 30468, 63 );

    m_OriginalTrainTrackData [0][536].Init ( -15412, -9384, 105, 30533, 191 );

    m_OriginalTrainTrackData [0][537].Init ( -15487, -9233, 109, 30596, 0 );

    m_OriginalTrainTrackData [0][538].Init ( -15553, -9084, 116, 30658, 113 );

    m_OriginalTrainTrackData [0][539].Init ( -15611, -8933, 124, 30718, 116 );

    m_OriginalTrainTrackData [0][540].Init ( -15663, -8782, 134, 30778, 117 );

    m_OriginalTrainTrackData [0][541].Init ( -15707, -8628, 145, 30838, 0 );

    m_OriginalTrainTrackData [0][542].Init ( -15743, -8470, 156, 30899, 111 );

    m_OriginalTrainTrackData [0][543].Init ( -15773, -8308, 166, 30961, 111 );

    m_OriginalTrainTrackData [0][544].Init ( -15797, -8140, 176, 31024, 21 );

    m_OriginalTrainTrackData [0][545].Init ( -15814, -7963, 185, 31091, 0 );

    m_OriginalTrainTrackData [0][546].Init ( -15825, -7781, 191, 31159, 23 );

    m_OriginalTrainTrackData [0][547].Init ( -15830, -7587, 196, 31232, 22 );

    m_OriginalTrainTrackData [0][548].Init ( -15831, -7382, 197, 31309, 22 );

    m_OriginalTrainTrackData [0][549].Init ( -15829, -7173, 197, 31387, 0 );

    m_OriginalTrainTrackData [0][550].Init ( -15828, -6964, 197, 31466, 201 );

    m_OriginalTrainTrackData [0][551].Init ( -15827, -6758, 197, 31543, 202 );

    m_OriginalTrainTrackData [0][552].Init ( -15827, -6552, 197, 31620, 212 );

    m_OriginalTrainTrackData [0][553].Init ( -15827, -6347, 197, 31697, 0 );

    m_OriginalTrainTrackData [0][554].Init ( -15827, -6144, 197, 31773, 208 );

    m_OriginalTrainTrackData [0][555].Init ( -15829, -5941, 197, 31849, 209 );

    m_OriginalTrainTrackData [0][556].Init ( -15830, -5739, 197, 31925, 72 );

    m_OriginalTrainTrackData [0][557].Init ( -15832, -5538, 197, 32001, 0 );

    m_OriginalTrainTrackData [0][558].Init ( -15834, -5337, 197, 32076, 117 );

    m_OriginalTrainTrackData [0][559].Init ( -15836, -5136, 197, 32151, 105 );

    m_OriginalTrainTrackData [0][560].Init ( -15838, -4936, 197, 32226, 106 );

    m_OriginalTrainTrackData [0][561].Init ( -15840, -4736, 199, 32301, 0 );

    m_OriginalTrainTrackData [0][562].Init ( -15841, -4688, 199, 32319, 126 );

    m_OriginalTrainTrackData [0][563].Init ( -15840, -4640, 199, 32337, 103 );

    m_OriginalTrainTrackData [0][564].Init ( -15841, -4592, 199, 32355, 154 );

    m_OriginalTrainTrackData [0][565].Init ( -15841, -4544, 197, 32373, 0 );

    m_OriginalTrainTrackData [0][566].Init ( -15840, -4416, 197, 32421, 133 );

    m_OriginalTrainTrackData [0][567].Init ( -15840, -4288, 197, 32469, 135 );

    m_OriginalTrainTrackData [0][568].Init ( -15839, -4160, 197, 32517, 82 );

    m_OriginalTrainTrackData [0][569].Init ( -15835, -4032, 197, 32565, 0 );

    m_OriginalTrainTrackData [0][570].Init ( -15826, -3904, 197, 32613, 149 );

    m_OriginalTrainTrackData [0][571].Init ( -15816, -3776, 197, 32662, 203 );

    m_OriginalTrainTrackData [0][572].Init ( -15803, -3648, 197, 32710, 215 );

    m_OriginalTrainTrackData [0][573].Init ( -15788, -3520, 197, 32758, 0 );

    m_OriginalTrainTrackData [0][574].Init ( -15774, -3392, 197, 32806, 78 );

    m_OriginalTrainTrackData [0][575].Init ( -15760, -3264, 197, 32855, 79 );

    m_OriginalTrainTrackData [0][576].Init ( -15748, -3136, 197, 32903, 79 );

    m_OriginalTrainTrackData [0][577].Init ( -15737, -3008, 197, 32951, 0 );

    m_OriginalTrainTrackData [0][578].Init ( -15729, -2880, 197, 32999, 199 );

    m_OriginalTrainTrackData [0][579].Init ( -15722, -2752, 197, 33047, 78 );

    m_OriginalTrainTrackData [0][580].Init ( -15712, -2624, 197, 33095, 65 );

    m_OriginalTrainTrackData [0][581].Init ( -15698, -2496, 197, 33144, 0 );

    m_OriginalTrainTrackData [0][582].Init ( -15680, -2368, 197, 33192, 78 );

    m_OriginalTrainTrackData [0][583].Init ( -15661, -2240, 197, 33241, 72 );

    m_OriginalTrainTrackData [0][584].Init ( -15641, -2112, 197, 33289, 199 );

    m_OriginalTrainTrackData [0][585].Init ( -15620, -1984, 197, 33338, 0 );

    m_OriginalTrainTrackData [0][586].Init ( -15604, -1856, 197, 33386, 105 );

    m_OriginalTrainTrackData [0][587].Init ( -15590, -1728, 197, 33435, 107 );

    m_OriginalTrainTrackData [0][588].Init ( -15578, -1600, 197, 33483, 116 );

    m_OriginalTrainTrackData [0][589].Init ( -15567, -1472, 197, 33531, 0 );

    m_OriginalTrainTrackData [0][590].Init ( -15558, -1344, 197, 33579, 207 );

    m_OriginalTrainTrackData [0][591].Init ( -15555, -1216, 197, 33627, 216 );

    m_OriginalTrainTrackData [0][592].Init ( -15555, -1088, 197, 33675, 76 );

    m_OriginalTrainTrackData [0][593].Init ( -15556, -320, 197, 33963, 0 );

    m_OriginalTrainTrackData [0][594].Init ( -15555, -95, 197, 34048, 196 );

    m_OriginalTrainTrackData [0][595].Init ( -15555, 991, 197, 34455, 77 );

    m_OriginalTrainTrackData [0][596].Init ( -15552, 1119, 197, 34503, 211 );

    m_OriginalTrainTrackData [0][597].Init ( -15548, 1247, 197, 34551, 0 );

    m_OriginalTrainTrackData [0][598].Init ( -15541, 1375, 197, 34599, 77 );

    m_OriginalTrainTrackData [0][599].Init ( -15533, 1502, 197, 34647, 76 );

    m_OriginalTrainTrackData [0][600].Init ( -15530, 1533, 197, 34658, 133 );

    m_OriginalTrainTrackData [0][601].Init ( -15516, 1660, 196, 34706, 0 );

    m_OriginalTrainTrackData [0][602].Init ( -15506, 1728, 192, 34732, 81 );

    m_OriginalTrainTrackData [0][603].Init ( -15485, 1854, 186, 34780, 127 );

    m_OriginalTrainTrackData [0][604].Init ( -15454, 1991, 174, 34833, 132 );

    m_OriginalTrainTrackData [0][605].Init ( -15415, 2107, 161, 34878, 0 );

    m_OriginalTrainTrackData [0][606].Init ( -15369, 2203, 147, 34918, 149 );

    m_OriginalTrainTrackData [0][607].Init ( -15300, 2291, 132, 34960, 149 );

    m_OriginalTrainTrackData [0][608].Init ( -15226, 2371, 117, 35001, 207 );

    m_OriginalTrainTrackData [0][609].Init ( -15149, 2441, 102, 35040, 0 );

    m_OriginalTrainTrackData [0][610].Init ( -15068, 2499, 88, 35078, 109 );

    m_OriginalTrainTrackData [0][611].Init ( -14981, 2554, 73, 35116, 120 );

    m_OriginalTrainTrackData [0][612].Init ( -14889, 2615, 57, 35158, 119 );

    m_OriginalTrainTrackData [0][613].Init ( -14788, 2683, 38, 35203, 0 );

    m_OriginalTrainTrackData [0][614].Init ( -14574, 2825, 2, 35300, 122 );

    m_OriginalTrainTrackData [0][615].Init ( -14469, 2897, -4, 35347, 106 );

    m_OriginalTrainTrackData [0][616].Init ( -14363, 2971, -3, 35396, 126 );

    m_OriginalTrainTrackData [0][617].Init ( -14258, 3045, 1, 35444, 0 );

    m_OriginalTrainTrackData [0][618].Init ( -14153, 3119, 8, 35492, 76 );

    m_OriginalTrainTrackData [0][619].Init ( -14049, 3194, 17, 35540, 77 );

    m_OriginalTrainTrackData [0][620].Init ( -13946, 3269, 30, 35588, 67 );

    m_OriginalTrainTrackData [0][621].Init ( -13842, 3344, 44, 35636, 0 );

    m_OriginalTrainTrackData [0][622].Init ( -13739, 3420, 59, 35684, 208 );

    m_OriginalTrainTrackData [0][623].Init ( -13635, 3495, 77, 35732, 125 );

    m_OriginalTrainTrackData [0][624].Init ( -13531, 3570, 95, 35780, 121 );

    m_OriginalTrainTrackData [0][625].Init ( -13428, 3645, 114, 35828, 0 );

    m_OriginalTrainTrackData [0][626].Init ( -13324, 3721, 133, 35876, 126 );

    m_OriginalTrainTrackData [0][627].Init ( -13221, 3796, 153, 35924, 129 );

    m_OriginalTrainTrackData [0][628].Init ( -13117, 3871, 171, 35972, 130 );

    m_OriginalTrainTrackData [0][629].Init ( -12910, 4022, 205, 36068, 0 );

    m_OriginalTrainTrackData [0][630].Init ( -12806, 4097, 223, 36116, 59 );

    m_OriginalTrainTrackData [0][631].Init ( -12599, 4247, 252, 36212, 195 );

    m_OriginalTrainTrackData [0][632].Init ( -12392, 4397, 267, 36308, 195 );

    m_OriginalTrainTrackData [0][633].Init ( -12184, 4548, 268, 36404, 0 );

    m_OriginalTrainTrackData [0][634].Init ( -11667, 4924, 268, 36644, 207 );

    m_OriginalTrainTrackData [0][635].Init ( -10321, 5902, 268, 37268, 128 );

    m_OriginalTrainTrackData [0][636].Init ( -8663, 7106, 268, 38036, 130 );

    m_OriginalTrainTrackData [0][637].Init ( -8043, 7557, 268, 38324, 0 );

    m_OriginalTrainTrackData [0][638].Init ( -7939, 7632, 268, 38372, 70 );

    m_OriginalTrainTrackData [0][639].Init ( -6653, 8569, 268, 38969, 126 );

    m_OriginalTrainTrackData [0][640].Init ( -6179, 8911, 260, 39188, 131 );

    m_OriginalTrainTrackData [0][641].Init ( -6085, 8979, 256, 39231, 0 );

    m_OriginalTrainTrackData [0][642].Init ( -5982, 9049, 251, 39278, 139 );

    m_OriginalTrainTrackData [0][643].Init ( -5872, 9121, 246, 39327, 148 );

    m_OriginalTrainTrackData [0][644].Init ( -5758, 9190, 241, 39377, 149 );

    m_OriginalTrainTrackData [0][645].Init ( -5609, 9271, 233, 39441, 0 );

    m_OriginalTrainTrackData [0][646].Init ( -5523, 9313, 229, 39477, 214 );

    m_OriginalTrainTrackData [0][647].Init ( -5404, 9361, 223, 39525, 100 );

    m_OriginalTrainTrackData [0][648].Init ( -5286, 9398, 218, 39571, 61 );

    m_OriginalTrainTrackData [0][649].Init ( -5139, 9429, 213, 39628, 0 );

    m_OriginalTrainTrackData [0][650].Init ( -4784, 9505, 207, 39764, 196 );

    m_OriginalTrainTrackData [0][651].Init ( -4510, 9564, 209, 39869, 69 );

    m_OriginalTrainTrackData [0][652].Init ( -3818, 9710, 227, 40134, 198 );

    m_OriginalTrainTrackData [0][653].Init ( -3500, 9782, 234, 40256, 0 );

    m_OriginalTrainTrackData [0][654].Init ( -3348, 9815, 236, 40315, 205 );

    m_OriginalTrainTrackData [0][655].Init ( -3196, 9848, 238, 40373, 128 );

    m_OriginalTrainTrackData [0][656].Init ( -3047, 9880, 238, 40430, 130 );

    m_OriginalTrainTrackData [0][657].Init ( -2941, 9903, 237, 40471, 0 );

    m_OriginalTrainTrackData [0][658].Init ( -2862, 9923, 236, 40501, 106 );

    m_OriginalTrainTrackData [0][659].Init ( -2607, 9991, 232, 40600, 59 );

    m_OriginalTrainTrackData [0][660].Init ( -2412, 10042, 228, 40676, 121 );

    m_OriginalTrainTrackData [0][661].Init ( -2305, 10070, 225, 40718, 0 );

    m_OriginalTrainTrackData [0][662].Init ( -2188, 10097, 222, 40763, 153 );

    m_OriginalTrainTrackData [0][663].Init ( -2062, 10124, 218, 40811, 86 );

    m_OriginalTrainTrackData [0][664].Init ( -1925, 10152, 212, 40863, 61 );

    m_OriginalTrainTrackData [0][665].Init ( -1775, 10179, 203, 40920, 0 );

    m_OriginalTrainTrackData [0][666].Init ( -1614, 10206, 191, 40982, 127 );

    m_OriginalTrainTrackData [0][667].Init ( -1438, 10233, 177, 41048, 124 );

    m_OriginalTrainTrackData [0][668].Init ( -1248, 10258, 164, 41120, 130 );

    m_OriginalTrainTrackData [0][669].Init ( -1042, 10282, 152, 41198, 0 );

    m_OriginalTrainTrackData [0][670].Init ( -875, 10300, 146, 41261, 143 );

    m_OriginalTrainTrackData [0][671].Init ( -718, 10315, 142, 41320, 143 );

    m_OriginalTrainTrackData [0][672].Init ( -567, 10328, 140, 41377, 56 );

    m_OriginalTrainTrackData [0][673].Init ( -423, 10337, 138, 41431, 0 );

    m_OriginalTrainTrackData [0][674].Init ( -286, 10345, 136, 41483, 218 );

    m_OriginalTrainTrackData [0][675].Init ( -153, 10348, 136, 41532, 219 );

    m_OriginalTrainTrackData [0][676].Init ( -26, 10349, 136, 41580, 218 );

    m_OriginalTrainTrackData [0][677].Init ( 96, 10347, 136, 41626, 0 );

    m_OriginalTrainTrackData [0][678].Init ( 216, 10342, 137, 41671, 207 );

    m_OriginalTrainTrackData [0][679].Init ( 332, 10333, 139, 41715, 207 );

    m_OriginalTrainTrackData [0][680].Init ( 446, 10321, 143, 41758, 207 );

    m_OriginalTrainTrackData [0][681].Init ( 557, 10305, 147, 41800, 0 );

    m_OriginalTrainTrackData [0][682].Init ( 667, 10285, 152, 41842, 220 );

    m_OriginalTrainTrackData [0][683].Init ( 776, 10261, 157, 41883, 219 );

    m_OriginalTrainTrackData [0][684].Init ( 885, 10234, 162, 41925, 240 );

    m_OriginalTrainTrackData [0][685].Init ( 993, 10201, 167, 41968, 0 );

    m_OriginalTrainTrackData [0][686].Init ( 1102, 10165, 170, 42011, 220 );

    m_OriginalTrainTrackData [0][687].Init ( 1212, 10125, 173, 42055, 162 );

    m_OriginalTrainTrackData [0][688].Init ( 1324, 10079, 174, 42100, 165 );

    m_OriginalTrainTrackData [0][689].Init ( 1438, 10029, 174, 42147, 0 );

    m_OriginalTrainTrackData [0][690].Init ( 1558, 9975, 174, 42196, 54 );

    m_OriginalTrainTrackData [0][691].Init ( 1681, 9925, 174, 42246, 121 );

    m_OriginalTrainTrackData [0][692].Init ( 1805, 9877, 174, 42296, 57 );

    m_OriginalTrainTrackData [0][693].Init ( 1930, 9834, 174, 42345, 0 );

    m_OriginalTrainTrackData [0][694].Init ( 2057, 9794, 174, 42395, 86 );

    m_OriginalTrainTrackData [0][695].Init ( 2184, 9758, 174, 42445, 70 );

    m_OriginalTrainTrackData [0][696].Init ( 2312, 9727, 174, 42494, 90 );

    m_OriginalTrainTrackData [0][697].Init ( 2440, 9700, 173, 42543, 0 );

    m_OriginalTrainTrackData [0][698].Init ( 2568, 9678, 172, 42592, 129 );

    m_OriginalTrainTrackData [0][699].Init ( 2696, 9660, 169, 42640, 127 );

    m_OriginalTrainTrackData [0][700].Init ( 2824, 9648, 165, 42689, 80 );

    m_OriginalTrainTrackData [0][701].Init ( 2951, 9640, 159, 42736, 0 );

    m_OriginalTrainTrackData [0][702].Init ( 3077, 9638, 153, 42784, 152 );

    m_OriginalTrainTrackData [0][703].Init ( 3202, 9641, 147, 42831, 137 );

    m_OriginalTrainTrackData [0][704].Init ( 3326, 9650, 140, 42877, 133 );

    m_OriginalTrainTrackData [0][705].Init ( 3449, 9665, 132, 42924, 0 );

    m_OriginalTrainTrackData [0][706].Init ( 3570, 9687, 125, 42970, 163 );

    m_OriginalTrainTrackData [0][707].Init ( 3689, 9714, 118, 43016, 138 );

    m_OriginalTrainTrackData [0][708].Init ( 3806, 9748, 112, 43061, 137 );

    m_OriginalTrainTrackData [0][709].Init ( 3920, 9789, 105, 43107, 0 );

    m_OriginalTrainTrackData [0][710].Init ( 4032, 9837, 100, 43152, 85 );

    m_OriginalTrainTrackData [0][711].Init ( 4141, 9893, 96, 43198, 89 );

    m_OriginalTrainTrackData [0][712].Init ( 4289, 9977, 91, 43262, 54 );

    m_OriginalTrainTrackData [0][713].Init ( 4447, 10067, 89, 43330, 0 );

    m_OriginalTrainTrackData [0][714].Init ( 4592, 10150, 87, 43393, 56 );

    m_OriginalTrainTrackData [0][715].Init ( 4723, 10225, 86, 43450, 95 );

    m_OriginalTrainTrackData [0][716].Init ( 4842, 10294, 86, 43501, 97 );

    m_OriginalTrainTrackData [0][717].Init ( 4952, 10358, 86, 43549, 0 );

    m_OriginalTrainTrackData [0][718].Init ( 5053, 10418, 86, 43593, 129 );

    m_OriginalTrainTrackData [0][719].Init ( 5146, 10475, 86, 43634, 92 );

    m_OriginalTrainTrackData [0][720].Init ( 5232, 10531, 86, 43672, 123 );

    m_OriginalTrainTrackData [0][721].Init ( 5314, 10586, 86, 43709, 0 );

    m_OriginalTrainTrackData [0][722].Init ( 5392, 10642, 86, 43745, 163 );

    m_OriginalTrainTrackData [0][723].Init ( 5467, 10700, 86, 43781, 164 );

    m_OriginalTrainTrackData [0][724].Init ( 5542, 10762, 86, 43817, 166 );

    m_OriginalTrainTrackData [0][725].Init ( 5613, 10823, 86, 43852, 0 );

    m_OriginalTrainTrackData [0][726].Init ( 5668, 10884, 86, 43883, 134 );

    m_OriginalTrainTrackData [0][727].Init ( 5717, 10955, 85, 43916, 35 );

    m_OriginalTrainTrackData [0][728].Init ( 5760, 11034, 85, 43949, 40 );

    m_OriginalTrainTrackData [0][729].Init ( 5797, 11123, 85, 43986, 0 );

    m_OriginalTrainTrackData [0][730].Init ( 5829, 11220, 84, 44024, 25 );

    m_OriginalTrainTrackData [0][731].Init ( 5856, 11326, 83, 44065, 51 );

    m_OriginalTrainTrackData [0][732].Init ( 5879, 11440, 82, 44108, 28 );

    m_OriginalTrainTrackData [0][733].Init ( 5897, 11561, 80, 44154, 0 );

    m_OriginalTrainTrackData [0][734].Init ( 5912, 11690, 79, 44203, 75 );

    m_OriginalTrainTrackData [0][735].Init ( 5924, 11826, 76, 44254, 91 );

    m_OriginalTrainTrackData [0][736].Init ( 5933, 11970, 74, 44308, 49 );

    m_OriginalTrainTrackData [0][737].Init ( 5939, 12120, 71, 44365, 0 );

    m_OriginalTrainTrackData [0][738].Init ( 5944, 12276, 68, 44423, 74 );

    m_OriginalTrainTrackData [0][739].Init ( 5946, 12439, 65, 44484, 74 );

    m_OriginalTrainTrackData [0][740].Init ( 5947, 12783, 57, 44613, 152 );

    m_OriginalTrainTrackData [0][741].Init ( 5947, 12924, 55, 44666, 0 );

    m_OriginalTrainTrackData [0][742].Init ( 5943, 13580, 43, 44912, 156 );

    m_OriginalTrainTrackData [0][743].Init ( 5940, 14298, 35, 45181, 157 );

    m_OriginalTrainTrackData [0][744].Init ( 5925, 16211, 45, 45899, 151 );

    m_OriginalTrainTrackData [0][745].Init ( 5914, 16910, 90, 46161, 0 );

    m_OriginalTrainTrackData [0][746].Init ( 5913, 17033, 97, 46207, 30 );

    m_OriginalTrainTrackData [0][747].Init ( 5914, 17149, 104, 46251, 63 );

    m_OriginalTrainTrackData [0][748].Init ( 5914, 17257, 110, 46291, 34 );

    m_OriginalTrainTrackData [0][749].Init ( 5911, 17357, 116, 46329, 0 );

    m_OriginalTrainTrackData [0][750].Init ( 5907, 17448, 120, 46363, 71 );

    m_OriginalTrainTrackData [0][751].Init ( 5904, 17530, 125, 46394, 22 );

    m_OriginalTrainTrackData [0][752].Init ( 5903, 17600, 128, 46420, 88 );

    m_OriginalTrainTrackData [0][753].Init ( 5902, 17662, 130, 46443, 0 );

    m_OriginalTrainTrackData [0][754].Init ( 5900, 17713, 131, 46462, 73 );

    m_OriginalTrainTrackData [0][755].Init ( 5895, 17788, 132, 46490, 34 );

    m_OriginalTrainTrackData [0][756].Init ( 5889, 17879, 134, 46525, 25 );

    m_OriginalTrainTrackData [0][757].Init ( 5878, 17978, 136, 46562, 0 );

    m_OriginalTrainTrackData [0][758].Init ( 5864, 18079, 138, 46600, 105 );

    m_OriginalTrainTrackData [0][759].Init ( 5850, 18194, 139, 46644, 137 );

    m_OriginalTrainTrackData [0][760].Init ( 5835, 18314, 141, 46689, 105 );

    m_OriginalTrainTrackData [0][761].Init ( 5822, 18439, 142, 46736, 0 );

    m_OriginalTrainTrackData [0][762].Init ( 5814, 18568, 144, 46785, 117 );

    m_OriginalTrainTrackData [0][763].Init ( 5808, 18701, 145, 46834, 66 );

    m_OriginalTrainTrackData [0][764].Init ( 5804, 18836, 146, 46885, 121 );

    m_OriginalTrainTrackData [0][765].Init ( 5804, 18973, 147, 46937, 0 );

    m_OriginalTrainTrackData [0][766].Init ( 5808, 19112, 149, 46989, 108 );

    m_OriginalTrainTrackData [0][767].Init ( 5818, 19250, 150, 47041, 136 );

    m_OriginalTrainTrackData [0][768].Init ( 5834, 19388, 151, 47093, 141 );

    m_OriginalTrainTrackData [0][769].Init ( 5855, 19525, 153, 47145, 0 );

    m_OriginalTrainTrackData [0][770].Init ( 5882, 19659, 154, 47196, 112 );

    m_OriginalTrainTrackData [0][771].Init ( 5918, 19791, 155, 47247, 166 );

    m_OriginalTrainTrackData [0][772].Init ( 5967, 19918, 156, 47298, 164 );

    m_OriginalTrainTrackData [0][773].Init ( 6030, 20063, 157, 47358, 0 );

    m_OriginalTrainTrackData [0][774].Init ( 6173, 20385, 158, 47490, 89 );

    m_OriginalTrainTrackData [0][775].Init ( 6237, 20525, 158, 47547, 92 );

    m_OriginalTrainTrackData [0][776].Init ( 6298, 20652, 158, 47600, 98 );

    m_OriginalTrainTrackData [0][777].Init ( 6355, 20768, 158, 47649, 0 );

    m_OriginalTrainTrackData [0][778].Init ( 6409, 20875, 158, 47694, 101 );

    m_OriginalTrainTrackData [0][779].Init ( 6459, 20974, 158, 47735, 88 );

    m_OriginalTrainTrackData [0][780].Init ( 6507, 21066, 158, 47774, 146 );

    m_OriginalTrainTrackData [0][781].Init ( 6552, 21152, 158, 47811, 0 );

    m_OriginalTrainTrackData [0][782].Init ( 6606, 21258, 158, 47855, 91 );

    m_OriginalTrainTrackData [0][783].Init ( 6633, 21308, 158, 47876, 151 );

    m_OriginalTrainTrackData [0][784].Init ( 6690, 21408, 158, 47920, 188 );

    m_OriginalTrainTrackData [0][785].Init ( 6756, 21495, 158, 47961, 0 );

    m_OriginalTrainTrackData [0][786].Init ( 6842, 21598, 158, 48011, 100 );

    m_OriginalTrainTrackData [0][787].Init ( 6928, 21697, 157, 48060, 74 );

    m_OriginalTrainTrackData [0][788].Init ( 7018, 21788, 155, 48108, 179 );

    m_OriginalTrainTrackData [0][789].Init ( 7112, 21871, 153, 48155, 0 );

    m_OriginalTrainTrackData [0][790].Init ( 7211, 21942, 151, 48201, 181 );

    m_OriginalTrainTrackData [0][791].Init ( 7316, 21999, 148, 48246, 99 );

    m_OriginalTrainTrackData [0][792].Init ( 7430, 22041, 146, 48291, 183 );

    m_OriginalTrainTrackData [0][793].Init ( 7548, 22064, 140, 48336, 0 );

    m_OriginalTrainTrackData [0][794].Init ( 7661, 22069, 136, 48379, 185 );

    m_OriginalTrainTrackData [0][795].Init ( 7738, 22068, 133, 48407, 184 );

    m_OriginalTrainTrackData [0][796].Init ( 7822, 22066, 130, 48439, 184 );

    m_OriginalTrainTrackData [0][797].Init ( 7916, 22065, 126, 48474, 0 );

    m_OriginalTrainTrackData [0][798].Init ( 7983, 22063, 122, 48499, 99 );

    m_OriginalTrainTrackData [0][799].Init ( 8065, 22050, 120, 48531, 187 );

    m_OriginalTrainTrackData [0][800].Init ( 8149, 22028, 117, 48563, 187 );

    m_OriginalTrainTrackData [0][801].Init ( 8234, 21995, 115, 48597, 0 );

    m_OriginalTrainTrackData [0][802].Init ( 8318, 21953, 113, 48632, 184 );

    m_OriginalTrainTrackData [0][803].Init ( 8404, 21903, 111, 48670, 181 );

    m_OriginalTrainTrackData [0][804].Init ( 8490, 21848, 109, 48708, 189 );

    m_OriginalTrainTrackData [0][805].Init ( 8577, 21780, 107, 48749, 0 );

    m_OriginalTrainTrackData [0][806].Init ( 8664, 21707, 105, 48792, 188 );

    m_OriginalTrainTrackData [0][807].Init ( 8751, 21636, 103, 48834, 178 );

    m_OriginalTrainTrackData [0][808].Init ( 8839, 21571, 101, 48875, 190 );

    m_OriginalTrainTrackData [0][809].Init ( 8926, 21502, 99, 48917, 0 );

    m_OriginalTrainTrackData [0][810].Init ( 9013, 21432, 97, 48959, 151 );

    m_OriginalTrainTrackData [0][811].Init ( 9099, 21363, 94, 49000, 102 );

    m_OriginalTrainTrackData [0][812].Init ( 9186, 21298, 92, 49041, 99 );

    m_OriginalTrainTrackData [0][813].Init ( 9271, 21239, 90, 49080, 0 );

    m_OriginalTrainTrackData [0][814].Init ( 9357, 21186, 88, 49117, 96 );

    m_OriginalTrainTrackData [0][815].Init ( 9442, 21147, 86, 49153, 93 );

    m_OriginalTrainTrackData [0][816].Init ( 9526, 21112, 84, 49187, 93 );

    m_OriginalTrainTrackData [0][817].Init ( 9608, 21085, 82, 49219, 0 );

    m_OriginalTrainTrackData [0][818].Init ( 9693, 21067, 80, 49252, 84 );

    m_OriginalTrainTrackData [0][819].Init ( 9775, 21058, 78, 49283, 89 );

    m_OriginalTrainTrackData [0][820].Init ( 9888, 21058, 78, 49325, 91 );

    m_OriginalTrainTrackData [0][821].Init ( 11196, 21058, 78, 49815, 0 );

    m_OriginalTrainTrackData [0][822].Init ( 11516, 21058, 78, 49935, 45 );

    m_OriginalTrainTrackData [0][823].Init ( 11716, 21058, 78, 50010, 45 );

    m_OriginalTrainTrackData [0][824].Init ( 11855, 21058, 78, 50063, 45 );

    m_OriginalTrainTrackData [0][825].Init ( 12785, 21058, 78, 50411, 0 );

    m_OriginalTrainTrackData [0][826].Init ( 12847, 21058, 78, 50435, 41 );

    m_OriginalTrainTrackData [0][827].Init ( 12863, 21059, 78, 50441, 43 );

    m_OriginalTrainTrackData [0][828].Init ( 12879, 21059, 78, 50447, 41 );

    m_OriginalTrainTrackData [0][829].Init ( 12895, 21062, 78, 50453, 0 );

    m_OriginalTrainTrackData [0][830].Init ( 12911, 21067, 78, 50459, 45 );

    m_OriginalTrainTrackData [0][831].Init ( 12926, 21072, 78, 50465, 45 );

    m_OriginalTrainTrackData [0][832].Init ( 12943, 21077, 78, 50472, 81 );

    m_OriginalTrainTrackData [0][833].Init ( 12958, 21082, 78, 50477, 0 );

    m_OriginalTrainTrackData [0][834].Init ( 12974, 21085, 78, 50484, 82 );

    m_OriginalTrainTrackData [0][835].Init ( 12991, 21087, 78, 50490, 45 );

    m_OriginalTrainTrackData [0][836].Init ( 13007, 21090, 78, 50496, 47 );

    m_OriginalTrainTrackData [0][837].Init ( 13039, 21089, 78, 50508, 0 );

    m_OriginalTrainTrackData [0][838].Init ( 13122, 21093, 78, 50539, 46 );

    m_OriginalTrainTrackData [0][839].Init ( 13206, 21101, 78, 50571, 81 );

    m_OriginalTrainTrackData [0][840].Init ( 13293, 21115, 78, 50604, 77 );

    m_OriginalTrainTrackData [0][841].Init ( 13381, 21133, 78, 50638, 0 );

    m_OriginalTrainTrackData [0][842].Init ( 13470, 21155, 78, 50672, 77 );

    m_OriginalTrainTrackData [0][843].Init ( 13560, 21179, 78, 50707, 85 );

    m_OriginalTrainTrackData [0][844].Init ( 13651, 21208, 78, 50743, 84 );

    m_OriginalTrainTrackData [0][845].Init ( 13742, 21237, 78, 50779, 0 );

    m_OriginalTrainTrackData [0][846].Init ( 13833, 21269, 78, 50815, 78 );

    m_OriginalTrainTrackData [0][847].Init ( 14016, 21335, 78, 50888, 83 );

    m_OriginalTrainTrackData [0][848].Init ( 14108, 21368, 78, 50924, 86 );

    m_OriginalTrainTrackData [0][849].Init ( 14198, 21400, 78, 50960, 0 );

    m_OriginalTrainTrackData [0][850].Init ( 14289, 21430, 78, 50996, 48 );

    m_OriginalTrainTrackData [0][851].Init ( 14379, 21459, 78, 51032, 97 );

    m_OriginalTrainTrackData [0][852].Init ( 14467, 21484, 78, 51066, 82 );

    m_OriginalTrainTrackData [0][853].Init ( 14555, 21508, 78, 51100, 0 );

    m_OriginalTrainTrackData [0][854].Init ( 14642, 21527, 78, 51133, 74 );

    m_OriginalTrainTrackData [0][855].Init ( 14728, 21541, 78, 51166, 189 );

    m_OriginalTrainTrackData [0][856].Init ( 14812, 21550, 78, 51198, 74 );

    m_OriginalTrainTrackData [0][857].Init ( 14893, 21553, 78, 51228, 0 );

    m_OriginalTrainTrackData [0][858].Init ( 15535, 21553, 78, 51469, 39 );

    m_OriginalTrainTrackData [0][859].Init ( 16175, 21553, 78, 51709, 42 );

    m_OriginalTrainTrackData [0][860].Init ( 16739, 21553, 80, 51920, 43 );

    m_OriginalTrainTrackData [0][861].Init ( 17199, 21553, 78, 52093, 0 );

    m_OriginalTrainTrackData [0][862].Init ( 17231, 21551, 78, 52105, 53 );

    m_OriginalTrainTrackData [0][863].Init ( 17247, 21549, 78, 52111, 28 );

    m_OriginalTrainTrackData [0][864].Init ( 17263, 21547, 78, 52117, 39 );

    m_OriginalTrainTrackData [0][865].Init ( 17279, 21545, 78, 52123, 0 );

    m_OriginalTrainTrackData [0][866].Init ( 17295, 21540, 78, 52129, 94 );

    m_OriginalTrainTrackData [0][867].Init ( 17311, 21535, 78, 52136, 94 );

    m_OriginalTrainTrackData [0][868].Init ( 17327, 21530, 78, 52142, 27 );

    m_OriginalTrainTrackData [0][869].Init ( 17343, 21526, 78, 52148, 0 );

    m_OriginalTrainTrackData [0][870].Init ( 17359, 21523, 78, 52154, 75 );

    m_OriginalTrainTrackData [0][871].Init ( 17375, 21522, 78, 52160, 75 );

    m_OriginalTrainTrackData [0][872].Init ( 17391, 21521, 78, 52166, 154 );

    m_OriginalTrainTrackData [0][873].Init ( 17455, 21521, 78, 52190, 0 );

    m_OriginalTrainTrackData [0][874].Init ( 17837, 21521, 78, 52334, 60 );

    m_OriginalTrainTrackData [0][875].Init ( 19375, 21521, 78, 52910, 32 );

    m_OriginalTrainTrackData [0][876].Init ( 19503, 21521, 78, 52958, 63 );

    m_OriginalTrainTrackData [0][877].Init ( 19588, 21516, 78, 52990, 0 );

    m_OriginalTrainTrackData [0][878].Init ( 19669, 21500, 78, 53021, 23 );

    m_OriginalTrainTrackData [0][879].Init ( 19746, 21476, 78, 53051, 70 );

    m_OriginalTrainTrackData [0][880].Init ( 19818, 21443, 78, 53081, 55 );

    m_OriginalTrainTrackData [0][881].Init ( 19886, 21403, 78, 53111, 0 );

    m_OriginalTrainTrackData [0][882].Init ( 19948, 21356, 78, 53140, 58 );

    m_OriginalTrainTrackData [0][883].Init ( 20007, 21302, 78, 53170, 28 );

    m_OriginalTrainTrackData [0][884].Init ( 20061, 21244, 78, 53200, 90 );

    m_OriginalTrainTrackData [0][885].Init ( 20110, 21182, 78, 53229, 0 );

    m_OriginalTrainTrackData [0][886].Init ( 20156, 21115, 78, 53260, 157 );

    m_OriginalTrainTrackData [0][887].Init ( 20197, 21046, 78, 53290, 151 );

    m_OriginalTrainTrackData [0][888].Init ( 20235, 20975, 78, 53320, 156 );

    m_OriginalTrainTrackData [0][889].Init ( 20268, 20902, 78, 53350, 0 );

    m_OriginalTrainTrackData [0][890].Init ( 20296, 20829, 78, 53379, 157 );

    m_OriginalTrainTrackData [0][891].Init ( 20322, 20755, 78, 53409, 175 );

    m_OriginalTrainTrackData [0][892].Init ( 20342, 20683, 78, 53437, 158 );

    m_OriginalTrainTrackData [0][893].Init ( 20359, 20611, 78, 53465, 0 );

    m_OriginalTrainTrackData [0][894].Init ( 20373, 20543, 78, 53491, 158 );

    m_OriginalTrainTrackData [0][895].Init ( 20382, 20476, 78, 53516, 171 );

    m_OriginalTrainTrackData [0][896].Init ( 20388, 20414, 78, 53539, 171 );

    m_OriginalTrainTrackData [0][897].Init ( 20390, 20355, 78, 53561, 0 );

    m_OriginalTrainTrackData [0][898].Init ( 20390, 20228, 78, 53609, 47 );

    m_OriginalTrainTrackData [0][899].Init ( 20390, 20196, 78, 53621, 47 );

    m_OriginalTrainTrackData [0][900].Init ( 20390, 20100, 78, 53657, 36 );

    m_OriginalTrainTrackData [0][901].Init ( 20389, 19971, 78, 53705, 0 );

    m_OriginalTrainTrackData [0][902].Init ( 20390, 19843, 78, 53753, 48 );

    m_OriginalTrainTrackData [0][903].Init ( 20390, 19827, 78, 53759, 53 );

    m_OriginalTrainTrackData [0][904].Init ( 20391, 19811, 78, 53765, 35 );

    m_OriginalTrainTrackData [0][905].Init ( 20394, 19795, 78, 53772, 0 );

    m_OriginalTrainTrackData [0][906].Init ( 20399, 19779, 78, 53778, 175 );

    m_OriginalTrainTrackData [0][907].Init ( 20404, 19763, 78, 53784, 172 );

    m_OriginalTrainTrackData [0][908].Init ( 20409, 19748, 78, 53790, 177 );

    m_OriginalTrainTrackData [0][909].Init ( 20413, 19732, 78, 53796, 0 );

    m_OriginalTrainTrackData [0][910].Init ( 20416, 19715, 78, 53803, 46 );

    m_OriginalTrainTrackData [0][911].Init ( 20418, 19699, 78, 53809, 47 );

    m_OriginalTrainTrackData [0][912].Init ( 20420, 19683, 78, 53815, 53 );

    m_OriginalTrainTrackData [0][913].Init ( 20421, 19667, 78, 53821, 0 );

    m_OriginalTrainTrackData [0][914].Init ( 20422, 19634, 78, 53833, 106 );

    m_OriginalTrainTrackData [0][915].Init ( 20422, 19526, 77, 53874, 105 );

    m_OriginalTrainTrackData [0][916].Init ( 20421, 19397, 74, 53922, 106 );

    m_OriginalTrainTrackData [0][917].Init ( 20421, 19268, 70, 53970, 0 );

    m_OriginalTrainTrackData [0][918].Init ( 20421, 19137, 63, 54020, 109 );

    m_OriginalTrainTrackData [0][919].Init ( 20421, 19017, 56, 54065, 109 );

    m_OriginalTrainTrackData [0][920].Init ( 20421, 18884, 45, 54114, 109 );

    m_OriginalTrainTrackData [0][921].Init ( 20422, 18759, 33, 54161, 0 );

    m_OriginalTrainTrackData [0][922].Init ( 20422, 18632, 20, 54209, 118 );

    m_OriginalTrainTrackData [0][923].Init ( 20422, 18594, 19, 54223, 116 );

    m_OriginalTrainTrackData [0][924].Init ( 20423, 18550, 19, 54240, 120 );

    m_OriginalTrainTrackData [0][925].Init ( 20428, 18460, 18, 54274, 0 );

    m_OriginalTrainTrackData [0][0].Init ( 20443, 18338, 18, 0, 0 );

    m_OriginalTrainTrackData [0][1].Init ( 20472, 18224, 15, 43, 0 );

    m_OriginalTrainTrackData [0][2].Init ( 20508, 18117, 12, 87, 0 );

    m_OriginalTrainTrackData [0][3].Init ( 20555, 18020, 9, 127, 0 );

    m_OriginalTrainTrackData [0][4].Init ( 20608, 17929, 4, 166, 97 );

    m_OriginalTrainTrackData [0][5].Init ( 20670, 17848, 0, 203, 96 );

    m_OriginalTrainTrackData [0][6].Init ( 20736, 17770, -4, 243, 163 );

    m_OriginalTrainTrackData [0][7].Init ( 20812, 17698, -9, 281, 97 );

    m_OriginalTrainTrackData [0][8].Init ( 20888, 17633, -13, 320, 160 );

    m_OriginalTrainTrackData [0][9].Init ( 20970, 17568, -17, 358, 137 );

    m_OriginalTrainTrackData [0][10].Init ( 21056, 17508, -21, 399, 137 );

    m_OriginalTrainTrackData [0][11].Init ( 21142, 17450, -24, 436, 96 );

    m_OriginalTrainTrackData [0][12].Init ( 21229, 17392, -26, 475, 94 );

    m_OriginalTrainTrackData [0][13].Init ( 21316, 17336, -28, 514, 97 );

    m_OriginalTrainTrackData [0][14].Init ( 21408, 17275, -28, 557, 96 );

    m_OriginalTrainTrackData [0][15].Init ( 21491, 17220, -27, 594, 94 );

    m_OriginalTrainTrackData [0][16].Init ( 21574, 17160, -25, 630, 13 );

    m_OriginalTrainTrackData [0][17].Init ( 21655, 17100, -22, 669, 128 );

    m_OriginalTrainTrackData [0][18].Init ( 21733, 17035, -19, 707, 219 );

    m_OriginalTrainTrackData [0][19].Init ( 21808, 16967, -15, 748, 13 );

    m_OriginalTrainTrackData [0][20].Init ( 21880, 16893, -10, 786, 96 );

    m_OriginalTrainTrackData [0][21].Init ( 21949, 16814, -5, 824, 219 );

    m_OriginalTrainTrackData [0][22].Init ( 22011, 16728, 0, 863, 13 );

    m_OriginalTrainTrackData [0][23].Init ( 22070, 16635, 5, 904, 64 );

    m_OriginalTrainTrackData [0][24].Init ( 22121, 16535, 10, 949, 219 );

    m_OriginalTrainTrackData [0][25].Init ( 22167, 16428, 14, 990, 40 );

    m_OriginalTrainTrackData [0][26].Init ( 22206, 16309, 19, 1038, 254 );

    m_OriginalTrainTrackData [0][27].Init ( 22236, 16184, 22, 1084, 179 );

    m_OriginalTrainTrackData [0][28].Init ( 22240, 16162, 23, 1094, 13 );

    m_OriginalTrainTrackData [0][29].Init ( 22261, 16046, 25, 1139, 232 );

    m_OriginalTrainTrackData [0][30].Init ( 22265, 16000, 26, 1155, 169 );

    m_OriginalTrainTrackData [0][31].Init ( 22274, 15895, 27, 1197, 13 );

    m_OriginalTrainTrackData [0][32].Init ( 22277, 15819, 28, 1224, 222 );

    m_OriginalTrainTrackData [0][33].Init ( 22279, 15758, 28, 1248, 13 );

    m_OriginalTrainTrackData [0][34].Init ( 22279, 15719, 28, 1263, 13 );

    m_OriginalTrainTrackData [0][35].Init ( 22278, 15695, 28, 1272, 254 );

    m_OriginalTrainTrackData [0][36].Init ( 22276, 15672, 28, 1278, 13 );

    m_OriginalTrainTrackData [0][37].Init ( 22268, 15642, 28, 1290, 1 );

    m_OriginalTrainTrackData [0][38].Init ( 22261, 15620, 28, 1300, 96 );

    m_OriginalTrainTrackData [0][39].Init ( 22252, 15591, 28, 1312, 219 );

    m_OriginalTrainTrackData [0][40].Init ( 22248, 15565, 28, 1321, 1 );

    m_OriginalTrainTrackData [0][41].Init ( 22248, 15548, 28, 1327, 128 );

    m_OriginalTrainTrackData [0][42].Init ( 22248, 15419, 28, 1375, 219 );

    m_OriginalTrainTrackData [0][43].Init ( 22248, 15318, 33, 1414, 148 );

    m_OriginalTrainTrackData [0][44].Init ( 22248, 15307, 34, 1417, 32 );

    m_OriginalTrainTrackData [0][45].Init ( 22248, 15193, 43, 1459, 219 );

    m_OriginalTrainTrackData [0][46].Init ( 22248, 15088, 55, 1498, 1 );

    m_OriginalTrainTrackData [0][47].Init ( 22248, 14986, 65, 1537, 64 );

    m_OriginalTrainTrackData [0][48].Init ( 22248, 14919, 71, 1564, 219 );

    m_OriginalTrainTrackData [0][49].Init ( 22248, 14826, 76, 1597, 1 );

    m_OriginalTrainTrackData [0][50].Init ( 22248, 14756, 78, 1624, 96 );

    m_OriginalTrainTrackData [0][51].Init ( 22248, 14679, 78, 1654, 219 );

    m_OriginalTrainTrackData [0][52].Init ( 22248, 14601, 78, 1681, 1 );

    m_OriginalTrainTrackData [0][53].Init ( 22248, 14529, 78, 1708, 96 );

    m_OriginalTrainTrackData [0][54].Init ( 22248, 14470, 78, 1732, 219 );

    m_OriginalTrainTrackData [0][55].Init ( 22247, 14392, 78, 1759, 1 );

    m_OriginalTrainTrackData [0][56].Init ( 22247, 14309, 78, 1792, 128 );

    m_OriginalTrainTrackData [0][57].Init ( 22247, 14218, 78, 1825, 219 );

    m_OriginalTrainTrackData [0][58].Init ( 22247, 14148, 78, 1852, 1 );

    m_OriginalTrainTrackData [0][59].Init ( 22247, 14065, 78, 1882, 96 );

    m_OriginalTrainTrackData [0][60].Init ( 22247, 13950, 78, 1927, 219 );

    m_OriginalTrainTrackData [0][61].Init ( 22247, 13878, 78, 1954, 238 );

    m_OriginalTrainTrackData [0][62].Init ( 22247, 13825, 78, 1972, 64 );

    m_OriginalTrainTrackData [0][63].Init ( 22247, 13780, 78, 1990, 219 );

    m_OriginalTrainTrackData [0][64].Init ( 22247, 13746, 78, 2002, 238 );

    m_OriginalTrainTrackData [0][65].Init ( 22247, 13728, 78, 2008, 64 );

    m_OriginalTrainTrackData [0][66].Init ( 22247, 13690, 78, 2023, 219 );

    m_OriginalTrainTrackData [0][67].Init ( 22247, 13658, 78, 2035, 1 );

    m_OriginalTrainTrackData [0][68].Init ( 22247, 13628, 78, 2047, 128 );

    m_OriginalTrainTrackData [0][69].Init ( 22247, 13600, 78, 2056, 219 );

    m_OriginalTrainTrackData [0][70].Init ( 22248, 13570, 78, 2068, 1 );

    m_OriginalTrainTrackData [0][71].Init ( 22248, 13526, 78, 2086, 96 );

    m_OriginalTrainTrackData [0][72].Init ( 22248, 13485, 78, 2101, 219 );

    m_OriginalTrainTrackData [0][73].Init ( 22248, 13440, 78, 2116, 1 );

    m_OriginalTrainTrackData [0][74].Init ( 22248, 13398, 78, 2134, 128 );

    m_OriginalTrainTrackData [0][75].Init ( 22248, 13360, 78, 2146, 219 );

    m_OriginalTrainTrackData [0][76].Init ( 22248, 13330, 78, 2158, 1 );

    m_OriginalTrainTrackData [0][77].Init ( 22248, 13289, 78, 2173, 96 );

    m_OriginalTrainTrackData [0][78].Init ( 22248, 13269, 78, 2182, 219 );

    m_OriginalTrainTrackData [0][79].Init ( 22249, 13235, 78, 2194, 1 );

    m_OriginalTrainTrackData [0][80].Init ( 22250, 13201, 78, 2206, 96 );

    m_OriginalTrainTrackData [0][81].Init ( 22254, 13171, 78, 2218, 219 );

    m_OriginalTrainTrackData [0][82].Init ( 22261, 13142, 78, 2231, 1 );

    m_OriginalTrainTrackData [0][83].Init ( 22277, 13114, 78, 2242, 96 );

    m_OriginalTrainTrackData [0][84].Init ( 22300, 13081, 78, 2257, 219 );

    m_OriginalTrainTrackData [0][85].Init ( 22337, 13033, 78, 2280, 1 );

    m_OriginalTrainTrackData [0][86].Init ( 22395, 12967, 78, 2314, 96 );

    m_OriginalTrainTrackData [0][87].Init ( 22445, 12906, 78, 2342, 219 );

    m_OriginalTrainTrackData [0][88].Init ( 22488, 12854, 78, 2370, 1 );

    m_OriginalTrainTrackData [0][89].Init ( 22535, 12795, 78, 2395, 96 );

    m_OriginalTrainTrackData [0][90].Init ( 22589, 12737, 78, 2425, 219 );

    m_OriginalTrainTrackData [0][91].Init ( 22639, 12674, 78, 2455, 1 );

    m_OriginalTrainTrackData [0][92].Init ( 22685, 12608, 78, 2485, 96 );

    m_OriginalTrainTrackData [0][93].Init ( 22725, 12538, 78, 2516, 219 );

    m_OriginalTrainTrackData [0][94].Init ( 22763, 12467, 78, 2547, 1 );

    m_OriginalTrainTrackData [0][95].Init ( 22796, 12394, 78, 2576, 96 );

    m_OriginalTrainTrackData [0][96].Init ( 22825, 12321, 78, 2606, 219 );

    m_OriginalTrainTrackData [0][97].Init ( 22850, 12248, 78, 2634, 1 );

    m_OriginalTrainTrackData [0][98].Init ( 22870, 12176, 78, 2662, 128 );

    m_OriginalTrainTrackData [0][99].Init ( 22888, 12105, 78, 2691, 219 );

    m_OriginalTrainTrackData [0][100].Init ( 22901, 12035, 78, 2718, 230 );

    m_OriginalTrainTrackData [0][101].Init ( 22910, 11969, 78, 2742, 64 );

    m_OriginalTrainTrackData [0][102].Init ( 22915, 11907, 78, 2766, 179 );

    m_OriginalTrainTrackData [0][103].Init ( 22918, 11865, 78, 2781, 1 );

    m_OriginalTrainTrackData [0][104].Init ( 22918, 11545, 78, 2901, 254 );

    m_OriginalTrainTrackData [0][105].Init ( 22918, 10714, 78, 3213, 219 );

    m_OriginalTrainTrackData [0][106].Init ( 22918, 10201, 78, 3405, 1 );

    m_OriginalTrainTrackData [0][107].Init ( 22918, 9913, 78, 3513, 254 );

    m_OriginalTrainTrackData [0][108].Init ( 22918, 9785, 78, 3561, 2 );

    m_OriginalTrainTrackData [0][109].Init ( 22904, 9704, 77, 3591, 1 );

    m_OriginalTrainTrackData [0][110].Init ( 22877, 9613, 77, 3629, 243 );

    m_OriginalTrainTrackData [0][111].Init ( 22839, 9507, 77, 3671, 189 );

    m_OriginalTrainTrackData [0][112].Init ( 22791, 9390, 77, 3719, 238 );

    m_OriginalTrainTrackData [0][113].Init ( 22734, 9265, 78, 3769, 128 );

    m_OriginalTrainTrackData [0][114].Init ( 22669, 9133, 78, 3825, 172 );

    m_OriginalTrainTrackData [0][115].Init ( 22598, 9000, 78, 3880, 238 );

    m_OriginalTrainTrackData [0][116].Init ( 22523, 8866, 78, 3938, 64 );

    m_OriginalTrainTrackData [0][117].Init ( 22444, 8735, 78, 3997, 172 );

    m_OriginalTrainTrackData [0][118].Init ( 22255, 8480, 87, 4115, 238 );

    m_OriginalTrainTrackData [0][119].Init ( 22191, 8391, 83, 4158, 96 );

    m_OriginalTrainTrackData [0][120].Init ( 22145, 8307, 82, 4192, 182 );

    m_OriginalTrainTrackData [0][121].Init ( 22121, 8233, 79, 4220, 1 );

    m_OriginalTrainTrackData [0][122].Init ( 22118, 8186, 79, 4239, 128 );

    m_OriginalTrainTrackData [0][123].Init ( 22118, 7866, 79, 4359, 182 );

    m_OriginalTrainTrackData [0][124].Init ( 22118, 5946, 79, 5079, 148 );

    m_OriginalTrainTrackData [0][125].Init ( 22122, 4229, 61, 5724, 32 );

    m_OriginalTrainTrackData [0][126].Init ( 22123, 3782, 58, 5892, 182 );

    m_OriginalTrainTrackData [0][127].Init ( 22119, 3542, 55, 5982, 110 );

    m_OriginalTrainTrackData [0][128].Init ( 22119, 3259, 53, 6087, 96 );

    m_OriginalTrainTrackData [0][129].Init ( 22123, 3030, 53, 6174, 182 );

    m_OriginalTrainTrackData [0][130].Init ( 22122, 2795, 53, 6261, 97 );

    m_OriginalTrainTrackData [0][131].Init ( 22121, 2586, 55, 6339, 160 );

    m_OriginalTrainTrackData [0][132].Init ( 22122, 2362, 58, 6423, 182 );

    m_OriginalTrainTrackData [0][133].Init ( 22138, 2253, 58, 6465, 1 );

    m_OriginalTrainTrackData [0][134].Init ( 22168, 2141, 63, 6509, 96 );

    m_OriginalTrainTrackData [0][135].Init ( 22250, 1916, 68, 6598, 182 );

    m_OriginalTrainTrackData [0][136].Init ( 22344, 1668, 83, 6698, 1 );

    m_OriginalTrainTrackData [0][137].Init ( 22412, 1448, 109, 6782, 96 );

    m_OriginalTrainTrackData [0][138].Init ( 22445, 1342, 124, 6826, 182 );

    m_OriginalTrainTrackData [0][139].Init ( 22484, 1229, 139, 6870, 1 );

    m_OriginalTrainTrackData [0][140].Init ( 22510, 1134, 153, 6908, 96 );

    m_OriginalTrainTrackData [0][141].Init ( 22535, 1026, 168, 6948, 182 );

    m_OriginalTrainTrackData [0][142].Init ( 22565, 842, 192, 7018, 1 );

    m_OriginalTrainTrackData [0][143].Init ( 22587, 682, 203, 7078, 32 );

    m_OriginalTrainTrackData [0][144].Init ( 22607, 513, 208, 7142, 182 );

    m_OriginalTrainTrackData [0][145].Init ( 22620, 290, 215, 7226, 1 );

    m_OriginalTrainTrackData [0][146].Init ( 22624, -643, 262, 7574, 96 );

    m_OriginalTrainTrackData [0][147].Init ( 22626, -741, 263, 7610, 73 );

    m_OriginalTrainTrackData [0][148].Init ( 22622, -834, 265, 7646, 1 );

    m_OriginalTrainTrackData [0][149].Init ( 22615, -896, 263, 7670, 32 );

    m_OriginalTrainTrackData [0][150].Init ( 22601, -961, 261, 7694, 73 );

    m_OriginalTrainTrackData [0][151].Init ( 22578, -1037, 256, 7723, 1 );

    m_OriginalTrainTrackData [0][152].Init ( 22542, -1129, 249, 7762, 160 );

    m_OriginalTrainTrackData [0][153].Init ( 22501, -1225, 241, 7801, 86 );

    m_OriginalTrainTrackData [0][154].Init ( 22455, -1333, 232, 7844, 110 );

    m_OriginalTrainTrackData [0][155].Init ( 22397, -1450, 220, 7893, 128 );

    m_OriginalTrainTrackData [0][156].Init ( 22344, -1557, 209, 7936, 182 );

    m_OriginalTrainTrackData [0][157].Init ( 22283, -1667, 197, 7985, 1 );

    m_OriginalTrainTrackData [0][158].Init ( 22218, -1786, 184, 8036, 173 );

    m_OriginalTrainTrackData [0][159].Init ( 22155, -1906, 170, 8087, 182 );

    m_OriginalTrainTrackData [0][160].Init ( 22104, -2001, 159, 8127, 148 );

    m_OriginalTrainTrackData [0][161].Init ( 22071, -2061, 152, 8153, 96 );

    m_OriginalTrainTrackData [0][162].Init ( 22042, -2108, 146, 8173, 182 );

    m_OriginalTrainTrackData [0][163].Init ( 22002, -2157, 141, 8196, 161 );

    m_OriginalTrainTrackData [0][164].Init ( 21955, -2196, 136, 8220, 160 );

    m_OriginalTrainTrackData [0][165].Init ( 21902, -2223, 132, 8243, 182 );

    m_OriginalTrainTrackData [0][166].Init ( 21842, -2244, 129, 8265, 1 );

    m_OriginalTrainTrackData [0][167].Init ( 21764, -2271, 124, 8297, 64 );

    m_OriginalTrainTrackData [0][168].Init ( 21562, -2321, 115, 8375, 182 );

    m_OriginalTrainTrackData [0][169].Init ( 21378, -2349, 107, 8444, 1 );

    m_OriginalTrainTrackData [0][170].Init ( 21193, -2362, 102, 8513, 96 );

    m_OriginalTrainTrackData [0][171].Init ( 20998, -2358, 100, 8588, 182 );

    m_OriginalTrainTrackData [0][172].Init ( 20804, -2342, 103, 8661, 1 );

    m_OriginalTrainTrackData [0][173].Init ( 20668, -2325, 108, 8712, 128 );

    m_OriginalTrainTrackData [0][174].Init ( 20533, -2301, 113, 8764, 219 );

    m_OriginalTrainTrackData [0][175].Init ( 20397, -2280, 117, 8815, 13 );

    m_OriginalTrainTrackData [0][176].Init ( 20263, -2262, 122, 8867, 96 );

    m_OriginalTrainTrackData [0][177].Init ( 20136, -2245, 125, 8912, 219 );

    m_OriginalTrainTrackData [0][178].Init ( 20062, -2236, 128, 8943, 13 );

    m_OriginalTrainTrackData [0][179].Init ( 19998, -2227, 129, 8967, 128 );

    m_OriginalTrainTrackData [0][180].Init ( 19740, -2199, 135, 9064, 219 );

    m_OriginalTrainTrackData [0][181].Init ( 19622, -2191, 137, 9109, 32 );

    m_OriginalTrainTrackData [0][182].Init ( 19492, -2182, 139, 9157, 96 );

    m_OriginalTrainTrackData [0][183].Init ( 19378, -2174, 140, 9199, 219 );

    m_OriginalTrainTrackData [0][184].Init ( 19255, -2175, 140, 9247, 13 );

    m_OriginalTrainTrackData [0][185].Init ( 19146, -2183, 146, 9286, 128 );

    m_OriginalTrainTrackData [0][186].Init ( 19031, -2206, 151, 9332, 219 );

    m_OriginalTrainTrackData [0][187].Init ( 18925, -2245, 166, 9374, 32 );

    m_OriginalTrainTrackData [0][188].Init ( 18826, -2287, 179, 9413, 32 );

    m_OriginalTrainTrackData [0][189].Init ( 18728, -2340, 197, 9454, 219 );

    m_OriginalTrainTrackData [0][190].Init ( 18633, -2400, 216, 9498, 32 );

    m_OriginalTrainTrackData [0][191].Init ( 18542, -2462, 235, 9539, 32 );

    m_OriginalTrainTrackData [0][192].Init ( 18446, -2526, 255, 9582, 219 );

    m_OriginalTrainTrackData [0][193].Init ( 18356, -2587, 273, 9623, 13 );

    m_OriginalTrainTrackData [0][194].Init ( 18261, -2649, 290, 9667, 128 );

    m_OriginalTrainTrackData [0][195].Init ( 18172, -2700, 300, 9704, 219 );

    m_OriginalTrainTrackData [0][196].Init ( 18071, -2750, 312, 9747, 13 );

    m_OriginalTrainTrackData [0][197].Init ( 17983, -2784, 320, 9783, 96 );

    m_OriginalTrainTrackData [0][198].Init ( 17883, -2816, 330, 9821, 219 );

    m_OriginalTrainTrackData [0][199].Init ( 17800, -2835, 342, 9852, 13 );

    m_OriginalTrainTrackData [0][200].Init ( 17707, -2851, 354, 9888, 96 );

    m_OriginalTrainTrackData [0][201].Init ( 17539, -2863, 381, 9951, 219 );

    m_OriginalTrainTrackData [0][202].Init ( 17377, -2862, 408, 10011, 32 );

    m_OriginalTrainTrackData [0][203].Init ( 17218, -2854, 431, 10072, 96 );

    m_OriginalTrainTrackData [0][204].Init ( 17057, -2850, 448, 10132, 81 );

    m_OriginalTrainTrackData [0][205].Init ( 16883, -2859, 465, 10198, 32 );

    m_OriginalTrainTrackData [0][206].Init ( 16787, -2871, 475, 10234, 173 );

    m_OriginalTrainTrackData [0][207].Init ( 16694, -2891, 485, 10271, 219 );

    m_OriginalTrainTrackData [0][208].Init ( 16599, -2918, 496, 10308, 32 );

    m_OriginalTrainTrackData [0][209].Init ( 16503, -2953, 507, 10347, 160 );

    m_OriginalTrainTrackData [0][210].Init ( 16412, -2999, 517, 10383, 219 );

    m_OriginalTrainTrackData [0][211].Init ( 16324, -3051, 527, 10422, 13 );

    m_OriginalTrainTrackData [0][212].Init ( 16244, -3121, 534, 10463, 96 );

    m_OriginalTrainTrackData [0][213].Init ( 16171, -3194, 541, 10501, 219 );

    m_OriginalTrainTrackData [0][214].Init ( 16109, -3288, 544, 10544, 32 );

    m_OriginalTrainTrackData [0][215].Init ( 16057, -3387, 547, 10584, 96 );

    m_OriginalTrainTrackData [0][216].Init ( 16015, -3491, 549, 10627, 219 );

    m_OriginalTrainTrackData [0][217].Init ( 15984, -3602, 551, 10670, 238 );

    m_OriginalTrainTrackData [0][218].Init ( 15962, -3698, 555, 10707, 96 );

    m_OriginalTrainTrackData [0][219].Init ( 15948, -3814, 559, 10750, 182 );

    m_OriginalTrainTrackData [0][220].Init ( 15945, -3915, 563, 10789, 1 );

    m_OriginalTrainTrackData [0][221].Init ( 15957, -4023, 569, 10828, 254 );

    m_OriginalTrainTrackData [0][222].Init ( 15982, -4131, 571, 10871, 246 );

    m_OriginalTrainTrackData [0][223].Init ( 16018, -4231, 573, 10910, 1 );

    m_OriginalTrainTrackData [0][224].Init ( 16077, -4336, 571, 10957, 64 );

    m_OriginalTrainTrackData [0][225].Init ( 16143, -4437, 568, 11000, 219 );

    m_OriginalTrainTrackData [0][226].Init ( 16237, -4546, 562, 11055, 41 );

    m_OriginalTrainTrackData [0][227].Init ( 16336, -4643, 555, 11108, 232 );

    m_OriginalTrainTrackData [0][228].Init ( 16456, -4748, 541, 11168, 219 );

    m_OriginalTrainTrackData [0][229].Init ( 16560, -4840, 529, 11221, 1 );

    m_OriginalTrainTrackData [0][230].Init ( 16670, -4932, 511, 11272, 222 );

    m_OriginalTrainTrackData [0][231].Init ( 16767, -5013, 496, 11319, 246 );

    m_OriginalTrainTrackData [0][232].Init ( 16872, -5097, 477, 11372, 1 );

    m_OriginalTrainTrackData [0][233].Init ( 16976, -5180, 458, 11422, 232 );

    m_OriginalTrainTrackData [0][234].Init ( 17087, -5266, 438, 11473, 219 );

    m_OriginalTrainTrackData [0][235].Init ( 17194, -5348, 416, 11524, 217 );

    m_OriginalTrainTrackData [0][236].Init ( 17342, -5459, 392, 11593, 222 );

    m_OriginalTrainTrackData [0][237].Init ( 17429, -5525, 375, 11634, 86 );

    m_OriginalTrainTrackData [0][238].Init ( 17480, -5564, 368, 11659, 57 );

    m_OriginalTrainTrackData [0][239].Init ( 17690, -5720, 339, 11758, 160 );

    m_OriginalTrainTrackData [0][240].Init ( 17790, -5813, 324, 11807, 123 );

    m_OriginalTrainTrackData [0][241].Init ( 17922, -5934, 306, 11875, 57 );

    m_OriginalTrainTrackData [0][242].Init ( 18081, -6163, 278, 11980, 96 );

    m_OriginalTrainTrackData [0][243].Init ( 18182, -6405, 256, 12077, 22 );

    m_OriginalTrainTrackData [0][244].Init ( 18239, -6658, 235, 12176, 233 );

    m_OriginalTrainTrackData [0][245].Init ( 18267, -6918, 220, 12272, 98 );

    m_OriginalTrainTrackData [0][246].Init ( 18279, -7182, 204, 12371, 160 );

    m_OriginalTrainTrackData [0][247].Init ( 18279, -7422, 205, 12461, 140 );

    m_OriginalTrainTrackData [0][248].Init ( 18279, -7682, 206, 12560, 100 );

    m_OriginalTrainTrackData [0][249].Init ( 18279, -7940, 207, 12656, 99 );

    m_OriginalTrainTrackData [0][250].Init ( 18279, -8182, 208, 12746, 204 );

    m_OriginalTrainTrackData [0][251].Init ( 18279, -8422, 208, 12836, 97 );

    m_OriginalTrainTrackData [0][252].Init ( 18279, -8682, 208, 12935, 220 );

    m_OriginalTrainTrackData [0][253].Init ( 18279, -9100, 206, 13091, 144 );

    m_OriginalTrainTrackData [0][254].Init ( 18279, -9292, 205, 13163, 98 );

    m_OriginalTrainTrackData [0][255].Init ( 18279, -9488, 198, 13238, 218 );

    m_OriginalTrainTrackData [0][256].Init ( 18279, -9744, 185, 13334, 172 );

    m_OriginalTrainTrackData [0][257].Init ( 18279, -10256, 185, 13526, 105 );

    m_OriginalTrainTrackData [0][258].Init ( 18279, -10384, 185, 13574, 224 );

    m_OriginalTrainTrackData [0][259].Init ( 18279, -10512, 185, 13622, 159 );

    m_OriginalTrainTrackData [0][260].Init ( 18279, -10640, 185, 13670, 2 );

    m_OriginalTrainTrackData [0][261].Init ( 18279, -10768, 185, 13718, 67 );

    m_OriginalTrainTrackData [0][262].Init ( 18279, -10896, 185, 13766, 202 );

    m_OriginalTrainTrackData [0][263].Init ( 18279, -10976, 185, 13796, 160 );

    m_OriginalTrainTrackData [0][264].Init ( 18278, -11032, 184, 13817, 16 );

    m_OriginalTrainTrackData [0][265].Init ( 18279, -11072, 184, 13832, 68 );

    m_OriginalTrainTrackData [0][266].Init ( 18279, -11112, 184, 13847, 10 );

    m_OriginalTrainTrackData [0][267].Init ( 18279, -11168, 184, 13868, 60 );

    m_OriginalTrainTrackData [0][268].Init ( 18275, -11295, 184, 13913, 192 );

    m_OriginalTrainTrackData [0][269].Init ( 18267, -11423, 183, 13961, 97 );

    m_OriginalTrainTrackData [0][270].Init ( 18252, -11549, 181, 14010, 16 );

    m_OriginalTrainTrackData [0][271].Init ( 18231, -11675, 178, 14059, 189 );

    m_OriginalTrainTrackData [0][272].Init ( 18205, -11792, 175, 14105, 94 );

    m_OriginalTrainTrackData [0][273].Init ( 18194, -11832, 174, 14120, 159 );

    m_OriginalTrainTrackData [0][274].Init ( 18185, -11872, 172, 14135, 1 );

    m_OriginalTrainTrackData [0][275].Init ( 18175, -11912, 172, 14151, 67 );

    m_OriginalTrainTrackData [0][276].Init ( 18163, -11952, 171, 14167, 146 );

    m_OriginalTrainTrackData [0][277].Init ( 18132, -12042, 166, 14202, 48 );

    m_OriginalTrainTrackData [0][278].Init ( 18086, -12159, 160, 14247, 208 );

    m_OriginalTrainTrackData [0][279].Init ( 18048, -12247, 154, 14283, 254 );

    m_OriginalTrainTrackData [0][280].Init ( 18002, -12361, 151, 14331, 201 );

    m_OriginalTrainTrackData [0][281].Init ( 17954, -12479, 145, 14377, 54 );

    m_OriginalTrainTrackData [0][282].Init ( 17856, -12716, 133, 14474, 12 );

    m_OriginalTrainTrackData [0][283].Init ( 17810, -12829, 126, 14519, 10 );

    m_OriginalTrainTrackData [0][284].Init ( 17759, -12946, 121, 14569, 9 );

    m_OriginalTrainTrackData [0][285].Init ( 17713, -13079, 116, 14619, 191 );

    m_OriginalTrainTrackData [0][286].Init ( 17701, -13125, 115, 14638, 65 );

    m_OriginalTrainTrackData [0][287].Init ( 17689, -13171, 114, 14656, 159 );

    m_OriginalTrainTrackData [0][288].Init ( 17676, -13218, 113, 14675, 2 );

    m_OriginalTrainTrackData [0][289].Init ( 17663, -13264, 111, 14694, 245 );

    m_OriginalTrainTrackData [0][290].Init ( 17634, -13373, 108, 14734, 248 );

    m_OriginalTrainTrackData [0][291].Init ( 17613, -13486, 105, 14777, 48 );

    m_OriginalTrainTrackData [0][292].Init ( 17595, -13621, 101, 14829, 0 );

    m_OriginalTrainTrackData [0][293].Init ( 17589, -13762, 99, 14883, 128 );

    m_OriginalTrainTrackData [0][294].Init ( 17588, -13810, 99, 14901, 144 );

    m_OriginalTrainTrackData [0][295].Init ( 17588, -13858, 99, 14919, 66 );

    m_OriginalTrainTrackData [0][296].Init ( 17588, -13906, 99, 14937, 129 );

    m_OriginalTrainTrackData [0][297].Init ( 17589, -13954, 99, 14955, 202 );

    m_OriginalTrainTrackData [0][298].Init ( 17589, -14082, 99, 15003, 4 );

    m_OriginalTrainTrackData [0][299].Init ( 17589, -14898, 102, 15309, 189 );

    m_OriginalTrainTrackData [0][300].Init ( 17589, -14931, 102, 15321, 5 );

    m_OriginalTrainTrackData [0][301].Init ( 17589, -15059, 102, 15369, 159 );

    m_OriginalTrainTrackData [0][302].Init ( 17589, -15115, 102, 15390, 251 );

    m_OriginalTrainTrackData [0][303].Init ( 17589, -15155, 102, 15405, 33 );

    m_OriginalTrainTrackData [0][304].Init ( 17589, -15195, 102, 15420, 204 );

    m_OriginalTrainTrackData [0][305].Init ( 17590, -15251, 102, 15441, 48 );

    m_OriginalTrainTrackData [0][306].Init ( 17583, -15306, 102, 15462, 208 );

    m_OriginalTrainTrackData [0][307].Init ( 17571, -15354, 102, 15480, 254 );

    m_OriginalTrainTrackData [0][308].Init ( 17550, -15404, 102, 15500, 11 );

    m_OriginalTrainTrackData [0][309].Init ( 17526, -15447, 102, 15518, 123 );

    m_OriginalTrainTrackData [0][310].Init ( 17493, -15490, 102, 15540, 94 );

    m_OriginalTrainTrackData [0][311].Init ( 17458, -15525, 102, 15557, 46 );

    m_OriginalTrainTrackData [0][312].Init ( 17409, -15564, 102, 15580, 20 );

    m_OriginalTrainTrackData [0][313].Init ( 17366, -15589, 102, 15600, 189 );

    m_OriginalTrainTrackData [0][314].Init ( 17316, -15610, 102, 15620, 101 );

    m_OriginalTrainTrackData [0][315].Init ( 17268, -15623, 102, 15638, 159 );

    m_OriginalTrainTrackData [0][316].Init ( 17215, -15630, 102, 15660, 249 );

    m_OriginalTrainTrackData [0][317].Init ( 17166, -15630, 102, 15678, 199 );

    m_OriginalTrainTrackData [0][318].Init ( 17069, -15630, 102, 15714, 16 );

    m_OriginalTrainTrackData [0][319].Init ( 16941, -15630, 102, 15762, 48 );

    m_OriginalTrainTrackData [0][320].Init ( 16813, -15630, 102, 15810, 75 );

    m_OriginalTrainTrackData [0][321].Init ( 16557, -15630, 102, 15906, 65 );

    m_OriginalTrainTrackData [0][322].Init ( 16429, -15630, 102, 15954, 153 );

    m_OriginalTrainTrackData [0][323].Init ( 16173, -15630, 102, 16050, 144 );

    m_OriginalTrainTrackData [0][324].Init ( 15917, -15630, 102, 16146, 126 );

    m_OriginalTrainTrackData [0][325].Init ( 15653, -15630, 101, 16245, 183 );

    m_OriginalTrainTrackData [0][326].Init ( 15517, -15630, 102, 16296, 160 );

    m_OriginalTrainTrackData [0][327].Init ( 15389, -15630, 102, 16344, 170 );

    m_OriginalTrainTrackData [0][328].Init ( 15024, -15630, 102, 16479, 9 );

    m_OriginalTrainTrackData [0][329].Init ( 14733, -15631, 100, 16590, 141 );

    m_OriginalTrainTrackData [0][330].Init ( 13800, -15630, 100, 16938, 87 );

    m_OriginalTrainTrackData [0][331].Init ( 13575, -15629, 100, 17025, 65 );

    m_OriginalTrainTrackData [0][332].Init ( 11994, -15630, 100, 17616, 190 );

    m_OriginalTrainTrackData [0][333].Init ( 11875, -15630, 100, 17661, 221 );

    m_OriginalTrainTrackData [0][334].Init ( 11780, -15632, 100, 17697, 208 );

    m_OriginalTrainTrackData [0][335].Init ( 11684, -15632, 100, 17733, 127 );

    m_OriginalTrainTrackData [0][336].Init ( 11588, -15630, 100, 17769, 206 );

    m_OriginalTrainTrackData [0][337].Init ( 11491, -15626, 97, 17805, 123 );

    m_OriginalTrainTrackData [0][338].Init ( 11392, -15618, 94, 17841, 121 );

    m_OriginalTrainTrackData [0][339].Init ( 11290, -15604, 89, 17880, 57 );

    m_OriginalTrainTrackData [0][340].Init ( 11187, -15585, 81, 17920, 142 );

    m_OriginalTrainTrackData [0][341].Init ( 11080, -15558, 69, 17961, 0 );

    m_OriginalTrainTrackData [0][342].Init ( 10970, -15523, 58, 18004, 136 );

    m_OriginalTrainTrackData [0][343].Init ( 10855, -15480, 41, 18052, 132 );

    m_OriginalTrainTrackData [0][344].Init ( 10737, -15426, 20, 18099, 8 );

    m_OriginalTrainTrackData [0][345].Init ( 10675, -15390, 10, 18127, 0 );

    m_OriginalTrainTrackData [0][346].Init ( 10607, -15342, 0, 18160, 17 );

    m_OriginalTrainTrackData [0][347].Init ( 10533, -15280, -8, 18194, 16 );

    m_OriginalTrainTrackData [0][348].Init ( 10454, -15209, -16, 18234, 250 );

    m_OriginalTrainTrackData [0][349].Init ( 10372, -15128, -22, 18277, 0 );

    m_OriginalTrainTrackData [0][350].Init ( 10286, -15038, -28, 18325, 234 );

    m_OriginalTrainTrackData [0][351].Init ( 10197, -14941, -30, 18374, 10 );

    m_OriginalTrainTrackData [0][352].Init ( 10104, -14839, -32, 18425, 19 );

    m_OriginalTrainTrackData [0][353].Init ( 10011, -14732, -35, 18478, 0 );

    m_OriginalTrainTrackData [0][354].Init ( 9796, -14486, -40, 18602, 224 );

    m_OriginalTrainTrackData [0][355].Init ( 9581, -14238, -42, 18725, 222 );

    m_OriginalTrainTrackData [0][356].Init ( 9365, -13985, -43, 18848, 247 );

    m_OriginalTrainTrackData [0][357].Init ( 9147, -13732, -43, 18974, 0 );

    m_OriginalTrainTrackData [0][358].Init ( 8927, -13476, -42, 19102, 249 );

    m_OriginalTrainTrackData [0][359].Init ( 8704, -13220, -40, 19227, 250 );

    m_OriginalTrainTrackData [0][360].Init ( 8480, -12963, -37, 19355, 1 );

    m_OriginalTrainTrackData [0][361].Init ( 8252, -12706, -34, 19484, 0 );

    m_OriginalTrainTrackData [0][362].Init ( 8021, -12449, -31, 19614, 224 );

    m_OriginalTrainTrackData [0][363].Init ( 7787, -12195, -27, 19743, 2 );

    m_OriginalTrainTrackData [0][364].Init ( 7550, -11943, -25, 19875, 7 );

    m_OriginalTrainTrackData [0][365].Init ( 7309, -11693, -22, 20004, 0 );

    m_OriginalTrainTrackData [0][366].Init ( 7063, -11447, -21, 20136, 7 );

    m_OriginalTrainTrackData [0][367].Init ( 6814, -11205, -20, 20265, 3 );

    m_OriginalTrainTrackData [0][368].Init ( 6688, -11088, -21, 20327, 3 );

    m_OriginalTrainTrackData [0][369].Init ( 6559, -10967, -21, 20397, 0 );

    m_OriginalTrainTrackData [0][370].Init ( 6519, -10932, -21, 20416, 11 );

    m_OriginalTrainTrackData [0][371].Init ( 6393, -10821, -20, 20478, 247 );

    m_OriginalTrainTrackData [0][372].Init ( 6225, -10669, -20, 20563, 101 );

    m_OriginalTrainTrackData [0][373].Init ( 6203, -10647, -20, 20575, 0 );

    m_OriginalTrainTrackData [0][374].Init ( 5840, -10340, -13, 20752, 7 );

    m_OriginalTrainTrackData [0][375].Init ( 5472, -10045, -1, 20929, 0 );

    m_OriginalTrainTrackData [0][376].Init ( 5098, -9766, 14, 21105, 4 );

    m_OriginalTrainTrackData [0][377].Init ( 4716, -9502, 32, 21280, 0 );

    m_OriginalTrainTrackData [0][378].Init ( 4327, -9256, 53, 21452, 0 );

    m_OriginalTrainTrackData [0][379].Init ( 3931, -9032, 74, 21621, 3 );

    m_OriginalTrainTrackData [0][380].Init ( 3523, -8828, 95, 21793, 7 );

    m_OriginalTrainTrackData [0][381].Init ( 3108, -8647, 116, 21964, 0 );

    m_OriginalTrainTrackData [0][382].Init ( 2683, -8491, 134, 22133, 131 );

    m_OriginalTrainTrackData [0][383].Init ( 2248, -8362, 150, 22302, 136 );

    m_OriginalTrainTrackData [0][384].Init ( 1802, -8261, 161, 22474, 133 );

    m_OriginalTrainTrackData [0][385].Init ( 1345, -8190, 168, 22647, 0 );

    m_OriginalTrainTrackData [0][386].Init ( 876, -8151, 169, 22825, 248 );

    m_OriginalTrainTrackData [0][387].Init ( 718, -8148, 167, 22885, 133 );

    m_OriginalTrainTrackData [0][388].Init ( 396, -8144, 163, 23005, 131 );

    m_OriginalTrainTrackData [0][389].Init ( 302, -8144, 160, 23041, 0 );

    m_OriginalTrainTrackData [0][390].Init ( 208, -8144, 159, 23074, 11 );

    m_OriginalTrainTrackData [0][391].Init ( 114, -8144, 159, 23110, 10 );

    m_OriginalTrainTrackData [0][392].Init ( 20, -8144, 159, 23146, 247 );

    m_OriginalTrainTrackData [0][393].Init ( -72, -8143, 158, 23179, 0 );

    m_OriginalTrainTrackData [0][394].Init ( -166, -8144, 156, 23212, 132 );

    m_OriginalTrainTrackData [0][395].Init ( -260, -8145, 154, 23248, 227 );

    m_OriginalTrainTrackData [0][396].Init ( -354, -8144, 150, 23284, 227 );

    m_OriginalTrainTrackData [0][397].Init ( -448, -8144, 145, 23320, 0 );

    m_OriginalTrainTrackData [0][398].Init ( -543, -8144, 137, 23353, 231 );

    m_OriginalTrainTrackData [0][399].Init ( -637, -8144, 127, 23389, 234 );

    m_OriginalTrainTrackData [0][400].Init ( -730, -8144, 114, 23425, 235 );

    m_OriginalTrainTrackData [0][401].Init ( -853, -8149, 98, 23470, 0 );

    m_OriginalTrainTrackData [0][402].Init ( -979, -8164, 88, 23518, 238 );

    m_OriginalTrainTrackData [0][403].Init ( -1106, -8188, 82, 23567, 243 );

    m_OriginalTrainTrackData [0][404].Init ( -1234, -8220, 81, 23617, 240 );

    m_OriginalTrainTrackData [0][405].Init ( -1361, -8260, 84, 23667, 0 );

    m_OriginalTrainTrackData [0][406].Init ( -1484, -8308, 89, 23716, 239 );

    m_OriginalTrainTrackData [0][407].Init ( -1601, -8362, 98, 23765, 228 );

    m_OriginalTrainTrackData [0][408].Init ( -1711, -8423, 108, 23809, 241 );

    m_OriginalTrainTrackData [0][409].Init ( -1813, -8488, 120, 23857, 0 );

    m_OriginalTrainTrackData [0][410].Init ( -1903, -8559, 132, 23898, 246 );

    m_OriginalTrainTrackData [0][411].Init ( -1980, -8634, 146, 23940, 239 );

    m_OriginalTrainTrackData [0][412].Init ( -2042, -8713, 159, 23979, 238 );

    m_OriginalTrainTrackData [0][413].Init ( -2098, -8795, 172, 24015, 0 );

    m_OriginalTrainTrackData [0][414].Init ( -2159, -8883, 187, 24054, 248 );

    m_OriginalTrainTrackData [0][415].Init ( -2224, -8975, 202, 24097, 243 );

    m_OriginalTrainTrackData [0][416].Init ( -2291, -9070, 217, 24140, 221 );

    m_OriginalTrainTrackData [0][417].Init ( -2362, -9166, 233, 24185, 0 );

    m_OriginalTrainTrackData [0][418].Init ( -2432, -9263, 248, 24230, 144 );

    m_OriginalTrainTrackData [0][419].Init ( -2503, -9359, 263, 24273, 144 );

    m_OriginalTrainTrackData [0][420].Init ( -2573, -9451, 276, 24318, 144 );

    m_OriginalTrainTrackData [0][421].Init ( -2641, -9539, 289, 24361, 0 );

    m_OriginalTrainTrackData [0][422].Init ( -2705, -9622, 299, 24400, 149 );

    m_OriginalTrainTrackData [0][423].Init ( -2765, -9698, 308, 24436, 38 );

    m_OriginalTrainTrackData [0][424].Init ( -2820, -9765, 314, 24468, 30 );

    m_OriginalTrainTrackData [0][425].Init ( -2878, -9824, 319, 24500, 0 );

    m_OriginalTrainTrackData [0][426].Init ( -2950, -9880, 322, 24534, 66 );

    m_OriginalTrainTrackData [0][427].Init ( -3034, -9931, 325, 24572, 30 );

    m_OriginalTrainTrackData [0][428].Init ( -3129, -9976, 326, 24612, 71 );

    m_OriginalTrainTrackData [0][429].Init ( -3230, -10012, 328, 24650, 0 );

    m_OriginalTrainTrackData [0][430].Init ( -3336, -10039, 328, 24693, 90 );

    m_OriginalTrainTrackData [0][431].Init ( -3446, -10055, 329, 24732, 95 );

    m_OriginalTrainTrackData [0][432].Init ( -3555, -10060, 329, 24774, 96 );

    m_OriginalTrainTrackData [0][433].Init ( -3662, -10051, 328, 24814, 0 );

    m_OriginalTrainTrackData [0][434].Init ( -3765, -10027, 328, 24854, 87 );

    m_OriginalTrainTrackData [0][435].Init ( -3861, -9988, 328, 24893, 27 );

    m_OriginalTrainTrackData [0][436].Init ( -3948, -9932, 328, 24932, 145 );

    m_OriginalTrainTrackData [0][437].Init ( -4027, -9872, 328, 24968, 0 );

    m_OriginalTrainTrackData [0][438].Init ( -4092, -9821, 328, 25000, 146 );

    m_OriginalTrainTrackData [0][439].Init ( -4149, -9778, 328, 25026, 147 );

    m_OriginalTrainTrackData [0][440].Init ( -4199, -9740, 328, 25049, 149 );

    m_OriginalTrainTrackData [0][441].Init ( -4245, -9704, 328, 25071, 0 );

    m_OriginalTrainTrackData [0][442].Init ( -4292, -9668, 328, 25094, 37 );

    m_OriginalTrainTrackData [0][443].Init ( -4342, -9629, 328, 25118, 34 );

    m_OriginalTrainTrackData [0][444].Init ( -4398, -9584, 328, 25144, 68 );

    m_OriginalTrainTrackData [0][445].Init ( -4465, -9531, 328, 25178, 0 );

    m_OriginalTrainTrackData [0][446].Init ( -4543, -9466, 328, 25214, 69 );

    m_OriginalTrainTrackData [0][447].Init ( -4639, -9388, 328, 25261, 44 );

    m_OriginalTrainTrackData [0][448].Init ( -4752, -9293, 328, 25319, 92 );

    m_OriginalTrainTrackData [0][449].Init ( -4876, -9197, 331, 25376, 0 );

    m_OriginalTrainTrackData [0][450].Init ( -4998, -9119, 339, 25430, 29 );

    m_OriginalTrainTrackData [0][451].Init ( -5116, -9058, 351, 25480, 71 );

    m_OriginalTrainTrackData [0][452].Init ( -5231, -9012, 366, 25526, 29 );

    m_OriginalTrainTrackData [0][453].Init ( -5343, -8982, 384, 25569, 0 );

    m_OriginalTrainTrackData [0][454].Init ( -5452, -8966, 403, 25612, 254 );

    m_OriginalTrainTrackData [0][455].Init ( -5558, -8964, 422, 25651, 254 );

    m_OriginalTrainTrackData [0][456].Init ( -5659, -8974, 441, 25690, 4 );

    m_OriginalTrainTrackData [0][457].Init ( -5757, -8996, 458, 25727, 0 );

    m_OriginalTrainTrackData [0][458].Init ( -5851, -9028, 472, 25765, 12 );

    m_OriginalTrainTrackData [0][459].Init ( -5940, -9071, 482, 25801, 11 );

    m_OriginalTrainTrackData [0][460].Init ( -6026, -9122, 488, 25840, 24 );

    m_OriginalTrainTrackData [0][461].Init ( -6105, -9175, 492, 25875, 0 );

    m_OriginalTrainTrackData [0][462].Init ( -6177, -9229, 496, 25909, 14 );

    m_OriginalTrainTrackData [0][463].Init ( -6243, -9283, 501, 25941, 13 );

    m_OriginalTrainTrackData [0][464].Init ( -6303, -9341, 506, 25971, 3 );

    m_OriginalTrainTrackData [0][465].Init ( -6356, -9404, 513, 26003, 0 );

    m_OriginalTrainTrackData [0][466].Init ( -6404, -9474, 520, 26035, 255 );

    m_OriginalTrainTrackData [0][467].Init ( -6447, -9554, 530, 26069, 5 );

    m_OriginalTrainTrackData [0][468].Init ( -6485, -9646, 540, 26105, 5 );

    m_OriginalTrainTrackData [0][469].Init ( -6520, -9750, 553, 26147, 0 );

    m_OriginalTrainTrackData [0][470].Init ( -6551, -9869, 567, 26193, 223 );

    m_OriginalTrainTrackData [0][471].Init ( -6579, -10007, 584, 26245, 18 );

    m_OriginalTrainTrackData [0][472].Init ( -6605, -10163, 602, 26306, 19 );

    m_OriginalTrainTrackData [0][473].Init ( -6628, -10329, 622, 26370, 0 );

    m_OriginalTrainTrackData [0][474].Init ( -6636, -10392, 630, 26394, 17 );

    m_OriginalTrainTrackData [0][475].Init ( -6650, -10492, 642, 26430, 22 );

    m_OriginalTrainTrackData [0][476].Init ( -6670, -10652, 662, 26491, 17 );

    m_OriginalTrainTrackData [0][477].Init ( -6693, -10807, 680, 26548, 0 );

    m_OriginalTrainTrackData [0][478].Init ( -6715, -10958, 697, 26606, 14 );

    m_OriginalTrainTrackData [0][479].Init ( -6743, -11101, 713, 26661, 15 );

    m_OriginalTrainTrackData [0][480].Init ( -6776, -11237, 728, 26714, 16 );

    m_OriginalTrainTrackData [0][481].Init ( -6814, -11364, 740, 26763, 0 );

    m_OriginalTrainTrackData [0][482].Init ( -6860, -11482, 750, 26812, 20 );

    m_OriginalTrainTrackData [0][483].Init ( -6915, -11590, 758, 26856, 2 );

    m_OriginalTrainTrackData [0][484].Init ( -6981, -11686, 762, 26899, 6 );

    m_OriginalTrainTrackData [0][485].Init ( -7058, -11770, 764, 26944, 0 );

    m_OriginalTrainTrackData [0][486].Init ( -7149, -11838, 761, 26985, 2 );

    m_OriginalTrainTrackData [0][487].Init ( -7254, -11892, 754, 27029, 7 );

    m_OriginalTrainTrackData [0][488].Init ( -7373, -11931, 741, 27076, 3 );

    m_OriginalTrainTrackData [0][489].Init ( -7504, -11959, 724, 27128, 0 );

    m_OriginalTrainTrackData [0][490].Init ( -7645, -11980, 703, 27180, 101 );

    m_OriginalTrainTrackData [0][491].Init ( -7793, -11993, 678, 27237, 20 );

    m_OriginalTrainTrackData [0][492].Init ( -7949, -12003, 650, 27294, 1 );

    m_OriginalTrainTrackData [0][493].Init ( -8107, -12012, 620, 27355, 0 );

    m_OriginalTrainTrackData [0][494].Init ( -8269, -12022, 586, 27415, 9 );

    m_OriginalTrainTrackData [0][495].Init ( -8431, -12036, 551, 27475, 9 );

    m_OriginalTrainTrackData [0][496].Init ( -8592, -12056, 513, 27539, 10 );

    m_OriginalTrainTrackData [0][497].Init ( -8751, -12085, 475, 27596, 0 );

    m_OriginalTrainTrackData [0][498].Init ( -8904, -12114, 435, 27657, 225 );

    m_OriginalTrainTrackData [0][499].Init ( -9058, -12133, 397, 27715, 3 );

    m_OriginalTrainTrackData [0][500].Init ( -9212, -12144, 360, 27772, 18 );

    m_OriginalTrainTrackData [0][501].Init ( -9367, -12144, 326, 27829, 0 );

    m_OriginalTrainTrackData [0][502].Init ( -9524, -12142, 293, 27889, 137 );

    m_OriginalTrainTrackData [0][503].Init ( -9684, -12135, 264, 27949, 110 );

    m_OriginalTrainTrackData [0][504].Init ( -9848, -12124, 238, 28012, 155 );

    m_OriginalTrainTrackData [0][505].Init ( -10017, -12113, 216, 28075, 0 );

    m_OriginalTrainTrackData [0][506].Init ( -10190, -12098, 199, 28139, 75 );

    m_OriginalTrainTrackData [0][507].Init ( -10371, -12087, 187, 28208, 121 );

    m_OriginalTrainTrackData [0][508].Init ( -10529, -12084, 181, 28268, 135 );

    m_OriginalTrainTrackData [0][509].Init ( -10752, -12079, 181, 28352, 0 );

    m_OriginalTrainTrackData [0][510].Init ( -10941, -12076, 180, 28421, 82 );

    m_OriginalTrainTrackData [0][511].Init ( -11137, -12073, 176, 28496, 145 );

    m_OriginalTrainTrackData [0][512].Init ( -11340, -12069, 171, 28571, 147 );

    m_OriginalTrainTrackData [0][513].Init ( -11548, -12063, 164, 28649, 0 );

    m_OriginalTrainTrackData [0][514].Init ( -11759, -12052, 155, 28727, 109 );

    m_OriginalTrainTrackData [0][515].Init ( -11972, -12038, 147, 28808, 104 );

    m_OriginalTrainTrackData [0][516].Init ( -12187, -12018, 138, 28890, 66 );

    m_OriginalTrainTrackData [0][517].Init ( -12401, -11992, 129, 28971, 0 );

    m_OriginalTrainTrackData [0][518].Init ( -12613, -11958, 121, 29050, 64 );

    m_OriginalTrainTrackData [0][519].Init ( -12821, -11915, 114, 29130, 191 );

    m_OriginalTrainTrackData [0][520].Init ( -13026, -11862, 108, 29211, 200 );

    m_OriginalTrainTrackData [0][521].Init ( -13222, -11798, 104, 29287, 0 );

    m_OriginalTrainTrackData [0][522].Init ( -13410, -11723, 103, 29363, 155 );

    m_OriginalTrainTrackData [0][523].Init ( -13593, -11627, 103, 29441, 75 );

    m_OriginalTrainTrackData [0][524].Init ( -13773, -11507, 103, 29521, 136 );

    m_OriginalTrainTrackData [0][525].Init ( -13950, -11364, 103, 29606, 0 );

    m_OriginalTrainTrackData [0][526].Init ( -14122, -11205, 103, 29696, 145 );

    m_OriginalTrainTrackData [0][527].Init ( -14289, -11030, 103, 29787, 145 );

    m_OriginalTrainTrackData [0][528].Init ( -14450, -10844, 103, 29878, 154 );

    m_OriginalTrainTrackData [0][529].Init ( -14603, -10652, 103, 29970, 0 );

    m_OriginalTrainTrackData [0][530].Init ( -14749, -10454, 103, 30063, 117 );

    m_OriginalTrainTrackData [0][531].Init ( -14885, -10258, 103, 30151, 117 );

    m_OriginalTrainTrackData [0][532].Init ( -15013, -10064, 103, 30237, 72 );

    m_OriginalTrainTrackData [0][533].Init ( -15129, -9877, 103, 30322, 0 );

    m_OriginalTrainTrackData [0][534].Init ( -15235, -9700, 103, 30399, 74 );

    m_OriginalTrainTrackData [0][535].Init ( -15329, -9537, 103, 30469, 63 );

    m_OriginalTrainTrackData [0][536].Init ( -15412, -9384, 105, 30533, 191 );

    m_OriginalTrainTrackData [0][537].Init ( -15487, -9233, 109, 30596, 0 );

    m_OriginalTrainTrackData [0][538].Init ( -15553, -9084, 116, 30659, 113 );

    m_OriginalTrainTrackData [0][539].Init ( -15611, -8933, 124, 30720, 116 );

    m_OriginalTrainTrackData [0][540].Init ( -15663, -8782, 134, 30780, 117 );

    m_OriginalTrainTrackData [0][541].Init ( -15707, -8628, 145, 30840, 0 );

    m_OriginalTrainTrackData [0][542].Init ( -15743, -8470, 156, 30901, 111 );

    m_OriginalTrainTrackData [0][543].Init ( -15773, -8308, 166, 30962, 111 );

    m_OriginalTrainTrackData [0][544].Init ( -15797, -8140, 176, 31026, 21 );

    m_OriginalTrainTrackData [0][545].Init ( -15814, -7963, 185, 31092, 0 );

    m_OriginalTrainTrackData [0][546].Init ( -15825, -7781, 191, 31161, 23 );

    m_OriginalTrainTrackData [0][547].Init ( -15830, -7587, 196, 31233, 22 );

    m_OriginalTrainTrackData [0][548].Init ( -15831, -7382, 197, 31311, 22 );

    m_OriginalTrainTrackData [0][549].Init ( -15829, -7173, 197, 31389, 0 );

    m_OriginalTrainTrackData [0][550].Init ( -15828, -6964, 197, 31467, 201 );

    m_OriginalTrainTrackData [0][551].Init ( -15827, -6758, 197, 31545, 202 );

    m_OriginalTrainTrackData [0][552].Init ( -15827, -6552, 197, 31620, 212 );

    m_OriginalTrainTrackData [0][553].Init ( -15827, -6347, 197, 31698, 0 );

    m_OriginalTrainTrackData [0][554].Init ( -15827, -6144, 197, 31773, 208 );

    m_OriginalTrainTrackData [0][555].Init ( -15829, -5941, 197, 31851, 209 );

    m_OriginalTrainTrackData [0][556].Init ( -15830, -5739, 197, 31926, 72 );

    m_OriginalTrainTrackData [0][557].Init ( -15832, -5538, 197, 32001, 0 );

    m_OriginalTrainTrackData [0][558].Init ( -15834, -5337, 197, 32076, 117 );

    m_OriginalTrainTrackData [0][559].Init ( -15836, -5136, 197, 32151, 105 );

    m_OriginalTrainTrackData [0][560].Init ( -15838, -4936, 197, 32226, 106 );

    m_OriginalTrainTrackData [0][561].Init ( -15840, -4736, 199, 32301, 0 );

    m_OriginalTrainTrackData [0][562].Init ( -15841, -4688, 199, 32319, 126 );

    m_OriginalTrainTrackData [0][563].Init ( -15840, -4640, 199, 32337, 103 );

    m_OriginalTrainTrackData [0][564].Init ( -15841, -4592, 199, 32355, 154 );

    m_OriginalTrainTrackData [0][565].Init ( -15841, -4544, 197, 32373, 0 );

    m_OriginalTrainTrackData [0][566].Init ( -15840, -4416, 197, 32421, 133 );

    m_OriginalTrainTrackData [0][567].Init ( -15840, -4288, 197, 32469, 135 );

    m_OriginalTrainTrackData [0][568].Init ( -15839, -4160, 197, 32518, 82 );

    m_OriginalTrainTrackData [0][569].Init ( -15835, -4032, 197, 32566, 0 );

    m_OriginalTrainTrackData [0][570].Init ( -15826, -3904, 197, 32614, 149 );

    m_OriginalTrainTrackData [0][571].Init ( -15816, -3776, 197, 32662, 203 );

    m_OriginalTrainTrackData [0][572].Init ( -15803, -3648, 197, 32710, 215 );

    m_OriginalTrainTrackData [0][573].Init ( -15788, -3520, 197, 32758, 0 );

    m_OriginalTrainTrackData [0][574].Init ( -15774, -3392, 197, 32807, 78 );

    m_OriginalTrainTrackData [0][575].Init ( -15760, -3264, 197, 32855, 79 );

    m_OriginalTrainTrackData [0][576].Init ( -15748, -3136, 197, 32903, 79 );

    m_OriginalTrainTrackData [0][577].Init ( -15737, -3008, 197, 32951, 0 );

    m_OriginalTrainTrackData [0][578].Init ( -15729, -2880, 197, 32999, 199 );

    m_OriginalTrainTrackData [0][579].Init ( -15722, -2752, 197, 33048, 78 );

    m_OriginalTrainTrackData [0][580].Init ( -15712, -2624, 197, 33096, 65 );

    m_OriginalTrainTrackData [0][581].Init ( -15698, -2496, 197, 33144, 0 );

    m_OriginalTrainTrackData [0][582].Init ( -15680, -2368, 197, 33192, 78 );

    m_OriginalTrainTrackData [0][583].Init ( -15661, -2240, 197, 33241, 72 );

    m_OriginalTrainTrackData [0][584].Init ( -15641, -2112, 197, 33290, 199 );

    m_OriginalTrainTrackData [0][585].Init ( -15620, -1984, 197, 33338, 0 );

    m_OriginalTrainTrackData [0][586].Init ( -15604, -1856, 197, 33387, 105 );

    m_OriginalTrainTrackData [0][587].Init ( -15590, -1728, 197, 33435, 107 );

    m_OriginalTrainTrackData [0][588].Init ( -15578, -1600, 197, 33483, 116 );

    m_OriginalTrainTrackData [0][589].Init ( -15567, -1472, 197, 33532, 0 );

    m_OriginalTrainTrackData [0][590].Init ( -15558, -1344, 197, 33580, 207 );

    m_OriginalTrainTrackData [0][591].Init ( -15555, -1216, 197, 33628, 216 );

    m_OriginalTrainTrackData [0][592].Init ( -15555, -1088, 197, 33676, 76 );

    m_OriginalTrainTrackData [0][593].Init ( -15556, -320, 197, 33964, 0 );

    m_OriginalTrainTrackData [0][594].Init ( -15555, -95, 197, 34051, 196 );

    m_OriginalTrainTrackData [0][595].Init ( -15555, 991, 197, 34453, 77 );

    m_OriginalTrainTrackData [0][596].Init ( -15552, 1119, 197, 34501, 211 );

    m_OriginalTrainTrackData [0][597].Init ( -15548, 1247, 197, 34549, 0 );

    m_OriginalTrainTrackData [0][598].Init ( -15541, 1375, 197, 34597, 77 );

    m_OriginalTrainTrackData [0][599].Init ( -15533, 1502, 197, 34645, 76 );

    m_OriginalTrainTrackData [0][600].Init ( -15530, 1533, 197, 34657, 133 );

    m_OriginalTrainTrackData [0][601].Init ( -15516, 1660, 196, 34705, 0 );

    m_OriginalTrainTrackData [0][602].Init ( -15506, 1728, 192, 34733, 81 );

    m_OriginalTrainTrackData [0][603].Init ( -15485, 1854, 186, 34778, 127 );

    m_OriginalTrainTrackData [0][604].Init ( -15454, 1991, 174, 34831, 132 );

    m_OriginalTrainTrackData [0][605].Init ( -15415, 2107, 161, 34878, 0 );

    m_OriginalTrainTrackData [0][606].Init ( -15369, 2203, 147, 34917, 149 );

    m_OriginalTrainTrackData [0][607].Init ( -15300, 2291, 132, 34960, 149 );

    m_OriginalTrainTrackData [0][608].Init ( -15226, 2371, 117, 35000, 207 );

    m_OriginalTrainTrackData [0][609].Init ( -15149, 2441, 102, 35041, 0 );

    m_OriginalTrainTrackData [0][610].Init ( -15068, 2499, 88, 35077, 109 );

    m_OriginalTrainTrackData [0][611].Init ( -14981, 2554, 73, 35116, 120 );

    m_OriginalTrainTrackData [0][612].Init ( -14889, 2615, 57, 35156, 119 );

    m_OriginalTrainTrackData [0][613].Init ( -14788, 2683, 38, 35203, 0 );

    m_OriginalTrainTrackData [0][614].Init ( -14574, 2825, 2, 35300, 122 );

    m_OriginalTrainTrackData [0][615].Init ( -14469, 2897, -4, 35348, 106 );

    m_OriginalTrainTrackData [0][616].Init ( -14363, 2971, -3, 35395, 126 );

    m_OriginalTrainTrackData [0][617].Init ( -14258, 3045, 1, 35443, 0 );

    m_OriginalTrainTrackData [0][618].Init ( -14153, 3119, 8, 35490, 76 );

    m_OriginalTrainTrackData [0][619].Init ( -14049, 3194, 17, 35539, 77 );

    m_OriginalTrainTrackData [0][620].Init ( -13946, 3269, 30, 35587, 67 );

    m_OriginalTrainTrackData [0][621].Init ( -13842, 3344, 44, 35636, 0 );

    m_OriginalTrainTrackData [0][622].Init ( -13739, 3420, 59, 35683, 208 );

    m_OriginalTrainTrackData [0][623].Init ( -13635, 3495, 77, 35731, 125 );

    m_OriginalTrainTrackData [0][624].Init ( -13531, 3570, 95, 35780, 121 );

    m_OriginalTrainTrackData [0][625].Init ( -13428, 3645, 114, 35827, 0 );

    m_OriginalTrainTrackData [0][626].Init ( -13324, 3721, 133, 35877, 126 );

    m_OriginalTrainTrackData [0][627].Init ( -13221, 3796, 153, 35924, 129 );

    m_OriginalTrainTrackData [0][628].Init ( -13117, 3871, 171, 35971, 130 );

    m_OriginalTrainTrackData [0][629].Init ( -12910, 4022, 205, 36068, 0 );

    m_OriginalTrainTrackData [0][630].Init ( -12806, 4097, 223, 36117, 59 );

    m_OriginalTrainTrackData [0][631].Init ( -12599, 4247, 252, 36212, 195 );

    m_OriginalTrainTrackData [0][632].Init ( -12392, 4397, 267, 36306, 195 );

    m_OriginalTrainTrackData [0][633].Init ( -12184, 4548, 268, 36403, 0 );

    m_OriginalTrainTrackData [0][634].Init ( -11667, 4924, 268, 36644, 207 );

    m_OriginalTrainTrackData [0][635].Init ( -10321, 5902, 268, 37266, 128 );

    m_OriginalTrainTrackData [0][636].Init ( -8663, 7106, 268, 38038, 130 );

    m_OriginalTrainTrackData [0][637].Init ( -8043, 7557, 268, 38323, 0 );

    m_OriginalTrainTrackData [0][638].Init ( -7939, 7632, 268, 38372, 70 );

    m_OriginalTrainTrackData [0][639].Init ( -6653, 8569, 268, 38969, 126 );

    m_OriginalTrainTrackData [0][640].Init ( -6179, 8911, 260, 39187, 131 );

    m_OriginalTrainTrackData [0][641].Init ( -6085, 8979, 256, 39232, 0 );

    m_OriginalTrainTrackData [0][642].Init ( -5982, 9049, 251, 39279, 139 );

    m_OriginalTrainTrackData [0][643].Init ( -5872, 9121, 246, 39327, 148 );

    m_OriginalTrainTrackData [0][644].Init ( -5758, 9190, 241, 39378, 149 );

    m_OriginalTrainTrackData [0][645].Init ( -5609, 9271, 233, 39439, 0 );

    m_OriginalTrainTrackData [0][646].Init ( -5523, 9313, 229, 39477, 214 );

    m_OriginalTrainTrackData [0][647].Init ( -5404, 9361, 223, 39525, 100 );

    m_OriginalTrainTrackData [0][648].Init ( -5286, 9398, 218, 39572, 61 );

    m_OriginalTrainTrackData [0][649].Init ( -5139, 9429, 213, 39627, 0 );

    m_OriginalTrainTrackData [0][650].Init ( -4784, 9505, 207, 39763, 196 );

    m_OriginalTrainTrackData [0][651].Init ( -4510, 9564, 209, 39870, 69 );

    m_OriginalTrainTrackData [0][652].Init ( -3818, 9710, 227, 40133, 198 );

    m_OriginalTrainTrackData [0][653].Init ( -3500, 9782, 234, 40256, 0 );

    m_OriginalTrainTrackData [0][654].Init ( -3348, 9815, 236, 40315, 205 );

    m_OriginalTrainTrackData [0][655].Init ( -3196, 9848, 238, 40374, 128 );

    m_OriginalTrainTrackData [0][656].Init ( -3047, 9880, 238, 40432, 130 );

    m_OriginalTrainTrackData [0][657].Init ( -2941, 9903, 237, 40471, 0 );

    m_OriginalTrainTrackData [0][658].Init ( -2862, 9923, 236, 40503, 106 );

    m_OriginalTrainTrackData [0][659].Init ( -2607, 9991, 232, 40602, 59 );

    m_OriginalTrainTrackData [0][660].Init ( -2412, 10042, 228, 40677, 121 );

    m_OriginalTrainTrackData [0][661].Init ( -2305, 10070, 225, 40717, 0 );

    m_OriginalTrainTrackData [0][662].Init ( -2188, 10097, 222, 40763, 153 );

    m_OriginalTrainTrackData [0][663].Init ( -2062, 10124, 218, 40812, 86 );

    m_OriginalTrainTrackData [0][664].Init ( -1925, 10152, 212, 40864, 61 );

    m_OriginalTrainTrackData [0][665].Init ( -1775, 10179, 203, 40922, 0 );

    m_OriginalTrainTrackData [0][666].Init ( -1614, 10206, 191, 40983, 127 );

    m_OriginalTrainTrackData [0][667].Init ( -1438, 10233, 177, 41050, 124 );

    m_OriginalTrainTrackData [0][668].Init ( -1248, 10258, 164, 41119, 130 );

    m_OriginalTrainTrackData [0][669].Init ( -1042, 10282, 152, 41198, 0 );

    m_OriginalTrainTrackData [0][670].Init ( -875, 10300, 146, 41261, 143 );

    m_OriginalTrainTrackData [0][671].Init ( -718, 10315, 142, 41322, 143 );

    m_OriginalTrainTrackData [0][672].Init ( -567, 10328, 140, 41379, 56 );

    m_OriginalTrainTrackData [0][673].Init ( -423, 10337, 138, 41433, 0 );

    m_OriginalTrainTrackData [0][674].Init ( -286, 10345, 136, 41484, 218 );

    m_OriginalTrainTrackData [0][675].Init ( -153, 10348, 136, 41532, 219 );

    m_OriginalTrainTrackData [0][676].Init ( -26, 10349, 136, 41580, 218 );

    m_OriginalTrainTrackData [0][677].Init ( 96, 10347, 136, 41625, 0 );

    m_OriginalTrainTrackData [0][678].Init ( 216, 10342, 137, 41670, 207 );

    m_OriginalTrainTrackData [0][679].Init ( 332, 10333, 139, 41712, 207 );

    m_OriginalTrainTrackData [0][680].Init ( 446, 10321, 143, 41754, 207 );

    m_OriginalTrainTrackData [0][681].Init ( 557, 10305, 147, 41797, 0 );

    m_OriginalTrainTrackData [0][682].Init ( 667, 10285, 152, 41840, 220 );

    m_OriginalTrainTrackData [0][683].Init ( 776, 10261, 157, 41883, 219 );

    m_OriginalTrainTrackData [0][684].Init ( 885, 10234, 162, 41923, 240 );

    m_OriginalTrainTrackData [0][685].Init ( 993, 10201, 167, 41966, 0 );

    m_OriginalTrainTrackData [0][686].Init ( 1102, 10165, 170, 42008, 220 );

    m_OriginalTrainTrackData [0][687].Init ( 1212, 10125, 173, 42053, 162 );

    m_OriginalTrainTrackData [0][688].Init ( 1324, 10079, 174, 42098, 165 );

    m_OriginalTrainTrackData [0][689].Init ( 1438, 10029, 174, 42144, 0 );

    m_OriginalTrainTrackData [0][690].Init ( 1558, 9975, 174, 42194, 54 );

    m_OriginalTrainTrackData [0][691].Init ( 1681, 9925, 174, 42245, 121 );

    m_OriginalTrainTrackData [0][692].Init ( 1805, 9877, 174, 42294, 57 );

    m_OriginalTrainTrackData [0][693].Init ( 1930, 9834, 174, 42344, 0 );

    m_OriginalTrainTrackData [0][694].Init ( 2057, 9794, 174, 42394, 86 );

    m_OriginalTrainTrackData [0][695].Init ( 2184, 9758, 174, 42444, 70 );

    m_OriginalTrainTrackData [0][696].Init ( 2312, 9727, 174, 42494, 90 );

    m_OriginalTrainTrackData [0][697].Init ( 2440, 9700, 173, 42543, 0 );

    m_OriginalTrainTrackData [0][698].Init ( 2568, 9678, 172, 42592, 129 );

    m_OriginalTrainTrackData [0][699].Init ( 2696, 9660, 169, 42640, 127 );

    m_OriginalTrainTrackData [0][700].Init ( 2824, 9648, 165, 42688, 80 );

    m_OriginalTrainTrackData [0][701].Init ( 2951, 9640, 159, 42733, 0 );

    m_OriginalTrainTrackData [0][702].Init ( 3077, 9638, 153, 42781, 152 );

    m_OriginalTrainTrackData [0][703].Init ( 3202, 9641, 147, 42829, 137 );

    m_OriginalTrainTrackData [0][704].Init ( 3326, 9650, 140, 42874, 133 );

    m_OriginalTrainTrackData [0][705].Init ( 3449, 9665, 132, 42923, 0 );

    m_OriginalTrainTrackData [0][706].Init ( 3570, 9687, 125, 42968, 163 );

    m_OriginalTrainTrackData [0][707].Init ( 3689, 9714, 118, 43015, 138 );

    m_OriginalTrainTrackData [0][708].Init ( 3806, 9748, 112, 43058, 137 );

    m_OriginalTrainTrackData [0][709].Init ( 3920, 9789, 105, 43106, 0 );

    m_OriginalTrainTrackData [0][710].Init ( 4032, 9837, 100, 43152, 85 );

    m_OriginalTrainTrackData [0][711].Init ( 4141, 9893, 96, 43196, 89 );

    m_OriginalTrainTrackData [0][712].Init ( 4289, 9977, 91, 43262, 54 );

    m_OriginalTrainTrackData [0][713].Init ( 4447, 10067, 89, 43328, 0 );

    m_OriginalTrainTrackData [0][714].Init ( 4592, 10150, 87, 43392, 56 );

    m_OriginalTrainTrackData [0][715].Init ( 4723, 10225, 86, 43449, 95 );

    m_OriginalTrainTrackData [0][716].Init ( 4842, 10294, 86, 43500, 97 );

    m_OriginalTrainTrackData [0][717].Init ( 4952, 10358, 86, 43548, 0 );

    m_OriginalTrainTrackData [0][718].Init ( 5053, 10418, 86, 43591, 129 );

    m_OriginalTrainTrackData [0][719].Init ( 5146, 10475, 86, 43633, 92 );

    m_OriginalTrainTrackData [0][720].Init ( 5232, 10531, 86, 43672, 123 );

    m_OriginalTrainTrackData [0][721].Init ( 5314, 10586, 86, 43709, 0 );

    m_OriginalTrainTrackData [0][722].Init ( 5392, 10642, 86, 43745, 163 );

    m_OriginalTrainTrackData [0][723].Init ( 5467, 10700, 86, 43779, 164 );

    m_OriginalTrainTrackData [0][724].Init ( 5542, 10762, 86, 43816, 166 );

    m_OriginalTrainTrackData [0][725].Init ( 5613, 10823, 86, 43850, 0 );

    m_OriginalTrainTrackData [0][726].Init ( 5668, 10884, 86, 43882, 134 );

    m_OriginalTrainTrackData [0][727].Init ( 5717, 10955, 85, 43914, 35 );

    m_OriginalTrainTrackData [0][728].Init ( 5760, 11034, 85, 43949, 40 );

    m_OriginalTrainTrackData [0][729].Init ( 5797, 11123, 85, 43984, 0 );

    m_OriginalTrainTrackData [0][730].Init ( 5829, 11220, 84, 44022, 25 );

    m_OriginalTrainTrackData [0][731].Init ( 5856, 11326, 83, 44063, 51 );

    m_OriginalTrainTrackData [0][732].Init ( 5879, 11440, 82, 44108, 28 );

    m_OriginalTrainTrackData [0][733].Init ( 5897, 11561, 80, 44154, 0 );

    m_OriginalTrainTrackData [0][734].Init ( 5912, 11690, 79, 44203, 75 );

    m_OriginalTrainTrackData [0][735].Init ( 5924, 11826, 76, 44254, 91 );

    m_OriginalTrainTrackData [0][736].Init ( 5933, 11970, 74, 44308, 49 );

    m_OriginalTrainTrackData [0][737].Init ( 5939, 12120, 71, 44365, 0 );

    m_OriginalTrainTrackData [0][738].Init ( 5944, 12276, 68, 44422, 74 );

    m_OriginalTrainTrackData [0][739].Init ( 5946, 12439, 65, 44482, 74 );

    m_OriginalTrainTrackData [0][740].Init ( 5947, 12783, 57, 44611, 152 );

    m_OriginalTrainTrackData [0][741].Init ( 5947, 12924, 55, 44665, 0 );

    m_OriginalTrainTrackData [0][742].Init ( 5943, 13580, 43, 44911, 156 );

    m_OriginalTrainTrackData [0][743].Init ( 5940, 14298, 35, 45181, 157 );

    m_OriginalTrainTrackData [0][744].Init ( 5925, 16211, 45, 45898, 151 );

    m_OriginalTrainTrackData [0][745].Init ( 5914, 16910, 90, 46159, 0 );

    m_OriginalTrainTrackData [0][746].Init ( 5913, 17033, 97, 46207, 30 );

    m_OriginalTrainTrackData [0][747].Init ( 5914, 17149, 104, 46249, 63 );

    m_OriginalTrainTrackData [0][748].Init ( 5914, 17257, 110, 46291, 34 );

    m_OriginalTrainTrackData [0][749].Init ( 5911, 17357, 116, 46327, 0 );

    m_OriginalTrainTrackData [0][750].Init ( 5907, 17448, 120, 46363, 71 );

    m_OriginalTrainTrackData [0][751].Init ( 5904, 17530, 125, 46393, 22 );

    m_OriginalTrainTrackData [0][752].Init ( 5903, 17600, 128, 46420, 88 );

    m_OriginalTrainTrackData [0][753].Init ( 5902, 17662, 130, 46441, 0 );

    m_OriginalTrainTrackData [0][754].Init ( 5900, 17713, 131, 46462, 73 );

    m_OriginalTrainTrackData [0][755].Init ( 5895, 17788, 132, 46489, 34 );

    m_OriginalTrainTrackData [0][756].Init ( 5889, 17879, 134, 46522, 25 );

    m_OriginalTrainTrackData [0][757].Init ( 5878, 17978, 136, 46562, 0 );

    m_OriginalTrainTrackData [0][758].Init ( 5864, 18079, 138, 46598, 105 );

    m_OriginalTrainTrackData [0][759].Init ( 5850, 18194, 139, 46643, 137 );

    m_OriginalTrainTrackData [0][760].Init ( 5835, 18314, 141, 46689, 105 );

    m_OriginalTrainTrackData [0][761].Init ( 5822, 18439, 142, 46734, 0 );

    m_OriginalTrainTrackData [0][762].Init ( 5814, 18568, 144, 46785, 117 );

    m_OriginalTrainTrackData [0][763].Init ( 5808, 18701, 145, 46833, 66 );

    m_OriginalTrainTrackData [0][764].Init ( 5804, 18836, 146, 46884, 121 );

    m_OriginalTrainTrackData [0][765].Init ( 5804, 18973, 147, 46935, 0 );

    m_OriginalTrainTrackData [0][766].Init ( 5808, 19112, 149, 46990, 108 );

    m_OriginalTrainTrackData [0][767].Init ( 5818, 19250, 150, 47041, 136 );

    m_OriginalTrainTrackData [0][768].Init ( 5834, 19388, 151, 47092, 141 );

    m_OriginalTrainTrackData [0][769].Init ( 5855, 19525, 153, 47143, 0 );

    m_OriginalTrainTrackData [0][770].Init ( 5882, 19659, 154, 47196, 112 );

    m_OriginalTrainTrackData [0][771].Init ( 5918, 19791, 155, 47245, 166 );

    m_OriginalTrainTrackData [0][772].Init ( 5967, 19918, 156, 47296, 164 );

    m_OriginalTrainTrackData [0][773].Init ( 6030, 20063, 157, 47356, 0 );

    m_OriginalTrainTrackData [0][774].Init ( 6173, 20385, 158, 47490, 89 );

    m_OriginalTrainTrackData [0][775].Init ( 6237, 20525, 158, 47546, 92 );

    m_OriginalTrainTrackData [0][776].Init ( 6298, 20652, 158, 47600, 98 );

    m_OriginalTrainTrackData [0][777].Init ( 6355, 20768, 158, 47650, 0 );

    m_OriginalTrainTrackData [0][778].Init ( 6409, 20875, 158, 47694, 101 );

    m_OriginalTrainTrackData [0][779].Init ( 6459, 20974, 158, 47734, 88 );

    m_OriginalTrainTrackData [0][780].Init ( 6507, 21066, 158, 47774, 146 );

    m_OriginalTrainTrackData [0][781].Init ( 6552, 21152, 158, 47812, 0 );

    m_OriginalTrainTrackData [0][782].Init ( 6606, 21258, 158, 47855, 91 );

    m_OriginalTrainTrackData [0][783].Init ( 6633, 21308, 158, 47877, 151 );

    m_OriginalTrainTrackData [0][784].Init ( 6690, 21408, 158, 47921, 188 );

    m_OriginalTrainTrackData [0][785].Init ( 6756, 21495, 158, 47959, 0 );

    m_OriginalTrainTrackData [0][786].Init ( 6842, 21598, 158, 48010, 100 );

    m_OriginalTrainTrackData [0][787].Init ( 6928, 21697, 157, 48061, 74 );

    m_OriginalTrainTrackData [0][788].Init ( 7018, 21788, 155, 48108, 179 );

    m_OriginalTrainTrackData [0][789].Init ( 7112, 21871, 153, 48155, 0 );

    m_OriginalTrainTrackData [0][790].Init ( 7211, 21942, 151, 48200, 181 );

    m_OriginalTrainTrackData [0][791].Init ( 7316, 21999, 148, 48244, 99 );

    m_OriginalTrainTrackData [0][792].Init ( 7430, 22041, 146, 48290, 183 );

    m_OriginalTrainTrackData [0][793].Init ( 7548, 22064, 140, 48336, 0 );

    m_OriginalTrainTrackData [0][794].Init ( 7661, 22069, 136, 48378, 185 );

    m_OriginalTrainTrackData [0][795].Init ( 7738, 22068, 133, 48408, 184 );

    m_OriginalTrainTrackData [0][796].Init ( 7822, 22066, 130, 48438, 184 );

    m_OriginalTrainTrackData [0][797].Init ( 7916, 22065, 126, 48474, 0 );

    m_OriginalTrainTrackData [0][798].Init ( 7983, 22063, 122, 48498, 99 );

    m_OriginalTrainTrackData [0][799].Init ( 8065, 22050, 120, 48531, 187 );

    m_OriginalTrainTrackData [0][800].Init ( 8149, 22028, 117, 48562, 187 );

    m_OriginalTrainTrackData [0][801].Init ( 8234, 21995, 115, 48598, 0 );

    m_OriginalTrainTrackData [0][802].Init ( 8318, 21953, 113, 48631, 184 );

    m_OriginalTrainTrackData [0][803].Init ( 8404, 21903, 111, 48670, 181 );

    m_OriginalTrainTrackData [0][804].Init ( 8490, 21848, 109, 48708, 189 );

    m_OriginalTrainTrackData [0][805].Init ( 8577, 21780, 107, 48750, 0 );

    m_OriginalTrainTrackData [0][806].Init ( 8664, 21707, 105, 48793, 188 );

    m_OriginalTrainTrackData [0][807].Init ( 8751, 21636, 103, 48833, 178 );

    m_OriginalTrainTrackData [0][808].Init ( 8839, 21571, 101, 48874, 190 );

    m_OriginalTrainTrackData [0][809].Init ( 8926, 21502, 99, 48917, 0 );

    m_OriginalTrainTrackData [0][810].Init ( 9013, 21432, 97, 48958, 151 );

    m_OriginalTrainTrackData [0][811].Init ( 9099, 21363, 94, 49000, 102 );

    m_OriginalTrainTrackData [0][812].Init ( 9186, 21298, 92, 49041, 99 );

    m_OriginalTrainTrackData [0][813].Init ( 9271, 21239, 90, 49080, 0 );

    m_OriginalTrainTrackData [0][814].Init ( 9357, 21186, 88, 49117, 96 );

    m_OriginalTrainTrackData [0][815].Init ( 9442, 21147, 86, 49153, 93 );

    m_OriginalTrainTrackData [0][816].Init ( 9526, 21112, 84, 49186, 93 );

    m_OriginalTrainTrackData [0][817].Init ( 9608, 21085, 82, 49221, 0 );

    m_OriginalTrainTrackData [0][818].Init ( 9693, 21067, 80, 49251, 84 );

    m_OriginalTrainTrackData [0][819].Init ( 9775, 21058, 78, 49282, 89 );

    m_OriginalTrainTrackData [0][820].Init ( 9888, 21058, 78, 49327, 91 );

    m_OriginalTrainTrackData [0][821].Init ( 11196, 21058, 78, 49816, 0 );

    m_OriginalTrainTrackData [0][822].Init ( 11516, 21058, 78, 49936, 45 );

    m_OriginalTrainTrackData [0][823].Init ( 11716, 21058, 78, 50011, 45 );

    m_OriginalTrainTrackData [0][824].Init ( 11855, 21058, 78, 50062, 45 );

    m_OriginalTrainTrackData [0][825].Init ( 12785, 21058, 78, 50413, 0 );

    m_OriginalTrainTrackData [0][826].Init ( 12847, 21058, 78, 50434, 41 );

    m_OriginalTrainTrackData [0][827].Init ( 12863, 21059, 78, 50440, 43 );

    m_OriginalTrainTrackData [0][828].Init ( 12879, 21059, 78, 50446, 41 );

    m_OriginalTrainTrackData [0][829].Init ( 12895, 21062, 78, 50452, 0 );

    m_OriginalTrainTrackData [0][830].Init ( 12911, 21067, 78, 50458, 45 );

    m_OriginalTrainTrackData [0][831].Init ( 12926, 21072, 78, 50465, 45 );

    m_OriginalTrainTrackData [0][832].Init ( 12943, 21077, 78, 50471, 81 );

    m_OriginalTrainTrackData [0][833].Init ( 12958, 21082, 78, 50478, 0 );

    m_OriginalTrainTrackData [0][834].Init ( 12974, 21085, 78, 50484, 82 );

    m_OriginalTrainTrackData [0][835].Init ( 12991, 21087, 78, 50490, 45 );

    m_OriginalTrainTrackData [0][836].Init ( 13007, 21090, 78, 50496, 47 );

    m_OriginalTrainTrackData [0][837].Init ( 13039, 21089, 78, 50508, 0 );

    m_OriginalTrainTrackData [0][838].Init ( 13122, 21093, 78, 50541, 46 );

    m_OriginalTrainTrackData [0][839].Init ( 13206, 21101, 78, 50572, 81 );

    m_OriginalTrainTrackData [0][840].Init ( 13293, 21115, 78, 50605, 77 );

    m_OriginalTrainTrackData [0][841].Init ( 13381, 21133, 78, 50639, 0 );

    m_OriginalTrainTrackData [0][842].Init ( 13470, 21155, 78, 50673, 77 );

    m_OriginalTrainTrackData [0][843].Init ( 13560, 21179, 78, 50710, 85 );

    m_OriginalTrainTrackData [0][844].Init ( 13651, 21208, 78, 50745, 84 );

    m_OriginalTrainTrackData [0][845].Init ( 13742, 21237, 78, 50779, 0 );

    m_OriginalTrainTrackData [0][846].Init ( 13833, 21269, 78, 50817, 78 );

    m_OriginalTrainTrackData [0][847].Init ( 14016, 21335, 78, 50890, 83 );

    m_OriginalTrainTrackData [0][848].Init ( 14108, 21368, 78, 50927, 86 );

    m_OriginalTrainTrackData [0][849].Init ( 14198, 21400, 78, 50962, 0 );

    m_OriginalTrainTrackData [0][850].Init ( 14289, 21430, 78, 50999, 48 );

    m_OriginalTrainTrackData [0][851].Init ( 14379, 21459, 78, 51034, 97 );

    m_OriginalTrainTrackData [0][852].Init ( 14467, 21484, 78, 51068, 82 );

    m_OriginalTrainTrackData [0][853].Init ( 14555, 21508, 78, 51102, 0 );

    m_OriginalTrainTrackData [0][854].Init ( 14642, 21527, 78, 51136, 74 );

    m_OriginalTrainTrackData [0][855].Init ( 14728, 21541, 78, 51169, 189 );

    m_OriginalTrainTrackData [0][856].Init ( 14812, 21550, 78, 51200, 74 );

    m_OriginalTrainTrackData [0][857].Init ( 14893, 21553, 78, 51230, 0 );

    m_OriginalTrainTrackData [0][858].Init ( 15535, 21553, 78, 51470, 39 );

    m_OriginalTrainTrackData [0][859].Init ( 16175, 21553, 78, 51710, 42 );

    m_OriginalTrainTrackData [0][860].Init ( 16739, 21553, 80, 51923, 43 );

    m_OriginalTrainTrackData [0][861].Init ( 17199, 21553, 78, 52094, 0 );

    m_OriginalTrainTrackData [0][862].Init ( 17231, 21551, 78, 52106, 53 );

    m_OriginalTrainTrackData [0][863].Init ( 17247, 21549, 78, 52112, 28 );

    m_OriginalTrainTrackData [0][864].Init ( 17263, 21547, 78, 52118, 39 );

    m_OriginalTrainTrackData [0][865].Init ( 17279, 21545, 78, 52124, 0 );

    m_OriginalTrainTrackData [0][866].Init ( 17295, 21540, 78, 52131, 94 );

    m_OriginalTrainTrackData [0][867].Init ( 17311, 21535, 78, 52137, 94 );

    m_OriginalTrainTrackData [0][868].Init ( 17327, 21530, 78, 52143, 27 );

    m_OriginalTrainTrackData [0][869].Init ( 17343, 21526, 78, 52150, 0 );

    m_OriginalTrainTrackData [0][870].Init ( 17359, 21523, 78, 52156, 75 );

    m_OriginalTrainTrackData [0][871].Init ( 17375, 21522, 78, 52162, 75 );

    m_OriginalTrainTrackData [0][872].Init ( 17391, 21521, 78, 52168, 154 );

    m_OriginalTrainTrackData [0][873].Init ( 17455, 21521, 78, 52192, 0 );

    m_OriginalTrainTrackData [0][874].Init ( 17837, 21521, 78, 52336, 60 );

    m_OriginalTrainTrackData [0][875].Init ( 19375, 21521, 78, 52912, 32 );

    m_OriginalTrainTrackData [0][876].Init ( 19503, 21521, 78, 52960, 63 );

    m_OriginalTrainTrackData [0][877].Init ( 19588, 21516, 78, 52993, 0 );

    m_OriginalTrainTrackData [0][878].Init ( 19669, 21500, 78, 53024, 23 );

    m_OriginalTrainTrackData [0][879].Init ( 19746, 21476, 78, 53055, 70 );

    m_OriginalTrainTrackData [0][880].Init ( 19818, 21443, 78, 53085, 55 );

    m_OriginalTrainTrackData [0][881].Init ( 19886, 21403, 78, 53113, 0 );

    m_OriginalTrainTrackData [0][882].Init ( 19948, 21356, 78, 53143, 58 );

    m_OriginalTrainTrackData [0][883].Init ( 20007, 21302, 78, 53173, 28 );

    m_OriginalTrainTrackData [0][884].Init ( 20061, 21244, 78, 53202, 90 );

    m_OriginalTrainTrackData [0][885].Init ( 20110, 21182, 78, 53232, 0 );

    m_OriginalTrainTrackData [0][886].Init ( 20156, 21115, 78, 53262, 157 );

    m_OriginalTrainTrackData [0][887].Init ( 20197, 21046, 78, 53293, 151 );

    m_OriginalTrainTrackData [0][888].Init ( 20235, 20975, 78, 53324, 156 );

    m_OriginalTrainTrackData [0][889].Init ( 20268, 20902, 78, 53354, 0 );

    m_OriginalTrainTrackData [0][890].Init ( 20296, 20829, 78, 53383, 157 );

    m_OriginalTrainTrackData [0][891].Init ( 20322, 20755, 78, 53412, 175 );

    m_OriginalTrainTrackData [0][892].Init ( 20342, 20683, 78, 53439, 158 );

    m_OriginalTrainTrackData [0][893].Init ( 20359, 20611, 78, 53467, 0 );

    m_OriginalTrainTrackData [0][894].Init ( 20373, 20543, 78, 53495, 158 );

    m_OriginalTrainTrackData [0][895].Init ( 20382, 20476, 78, 53519, 171 );

    m_OriginalTrainTrackData [0][896].Init ( 20388, 20414, 78, 53543, 171 );

    m_OriginalTrainTrackData [0][897].Init ( 20390, 20355, 78, 53564, 0 );

    m_OriginalTrainTrackData [0][898].Init ( 20390, 20228, 78, 53612, 47 );

    m_OriginalTrainTrackData [0][899].Init ( 20390, 20196, 78, 53624, 47 );

    m_OriginalTrainTrackData [0][900].Init ( 20390, 20100, 78, 53660, 36 );

    m_OriginalTrainTrackData [0][901].Init ( 20389, 19971, 78, 53708, 0 );

    m_OriginalTrainTrackData [0][902].Init ( 20390, 19843, 78, 53756, 48 );

    m_OriginalTrainTrackData [0][903].Init ( 20390, 19827, 78, 53762, 53 );

    m_OriginalTrainTrackData [0][904].Init ( 20391, 19811, 78, 53768, 35 );

    m_OriginalTrainTrackData [0][905].Init ( 20394, 19795, 78, 53775, 0 );

    m_OriginalTrainTrackData [0][906].Init ( 20399, 19779, 78, 53781, 175 );

    m_OriginalTrainTrackData [0][907].Init ( 20404, 19763, 78, 53788, 172 );

    m_OriginalTrainTrackData [0][908].Init ( 20409, 19748, 78, 53794, 177 );

    m_OriginalTrainTrackData [0][909].Init ( 20413, 19732, 78, 53800, 0 );

    m_OriginalTrainTrackData [0][910].Init ( 20416, 19715, 78, 53807, 46 );

    m_OriginalTrainTrackData [0][911].Init ( 20418, 19699, 78, 53813, 47 );

    m_OriginalTrainTrackData [0][912].Init ( 20420, 19683, 78, 53819, 53 );

    m_OriginalTrainTrackData [0][913].Init ( 20421, 19667, 78, 53825, 0 );

    m_OriginalTrainTrackData [0][914].Init ( 20422, 19634, 78, 53837, 106 );

    m_OriginalTrainTrackData [0][915].Init ( 20422, 19526, 77, 53879, 105 );

    m_OriginalTrainTrackData [0][916].Init ( 20421, 19397, 74, 53927, 106 );

    m_OriginalTrainTrackData [0][917].Init ( 20421, 19268, 70, 53975, 0 );

    m_OriginalTrainTrackData [0][918].Init ( 20421, 19137, 63, 54023, 109 );

    m_OriginalTrainTrackData [0][919].Init ( 20421, 19017, 56, 54068, 109 );

    m_OriginalTrainTrackData [0][920].Init ( 20421, 18884, 45, 54119, 109 );

    m_OriginalTrainTrackData [0][921].Init ( 20422, 18759, 33, 54167, 0 );

    m_OriginalTrainTrackData [0][922].Init ( 20422, 18632, 20, 54212, 118 );

    m_OriginalTrainTrackData [0][923].Init ( 20422, 18594, 19, 54227, 116 );

    m_OriginalTrainTrackData [0][924].Init ( 20423, 18550, 19, 54245, 120 );

    m_OriginalTrainTrackData [0][925].Init ( 20428, 18460, 18, 54278, 0 );
    m_OriginalTrainTrackData [1][0].Init ( -18029, 317, 273, 0, 32 );

    m_OriginalTrainTrackData [1][1].Init ( -18035, 157, 273, 60, 54 );

    m_OriginalTrainTrackData [1][2].Init ( -18039, -2, 273, 119, 53 );

    m_OriginalTrainTrackData [1][3].Init ( -18044, -162, 273, 179, 32 );

    m_OriginalTrainTrackData [1][4].Init ( -18044, -322, 273, 239, 54 );

    m_OriginalTrainTrackData [1][5].Init ( -18044, -482, 273, 299, 52 );

    m_OriginalTrainTrackData [1][6].Init ( -18035, -506, 273, 309, 52 );

    m_OriginalTrainTrackData [1][7].Init ( -18034, -509, 273, 310, 46 );

    m_OriginalTrainTrackData [1][8].Init ( -18021, -530, 273, 319, 46 );

    m_OriginalTrainTrackData [1][9].Init ( -17995, -548, 273, 331, 54 );

    m_OriginalTrainTrackData [1][10].Init ( -17975, -559, 273, 340, 44 );

    m_OriginalTrainTrackData [1][11].Init ( -17950, -561, 273, 349, 52 );

    m_OriginalTrainTrackData [1][12].Init ( -17897, -561, 273, 369, 54 );

    m_OriginalTrainTrackData [1][13].Init ( -17577, -560, 273, 489, 57 );

    m_OriginalTrainTrackData [1][14].Init ( -17399, -560, 273, 556, 46 );

    m_OriginalTrainTrackData [1][15].Init ( -17374, -553, 273, 565, 49 );

    m_OriginalTrainTrackData [1][16].Init ( -17359, -542, 273, 572, 46 );

    m_OriginalTrainTrackData [1][17].Init ( -17345, -527, 273, 580, 83 );

    m_OriginalTrainTrackData [1][18].Init ( -17336, -509, 273, 588, 32 );

    m_OriginalTrainTrackData [1][19].Init ( -17333, -482, 273, 598, 49 );

    m_OriginalTrainTrackData [1][20].Init ( -17333, -317, 273, 660, 57 );

    m_OriginalTrainTrackData [1][21].Init ( -17333, -157, 273, 720, 51 );

    m_OriginalTrainTrackData [1][22].Init ( -17333, 2, 273, 779, 55 );

    m_OriginalTrainTrackData [1][23].Init ( -17332, 162, 273, 839, 46 );

    m_OriginalTrainTrackData [1][24].Init ( -17326, 189, 273, 850, 83 );

    m_OriginalTrainTrackData [1][25].Init ( -17312, 217, 273, 861, 32 );

    m_OriginalTrainTrackData [1][26].Init ( -17290, 237, 273, 873, 49 );

    m_OriginalTrainTrackData [1][27].Init ( -17257, 244, 273, 885, 48 );

    m_OriginalTrainTrackData [1][28].Init ( -17097, 243, 273, 945, 57 );

    m_OriginalTrainTrackData [1][29].Init ( -16937, 243, 273, 1005, 46 );

    m_OriginalTrainTrackData [1][30].Init ( -16777, 243, 273, 1065, 54 );

    m_OriginalTrainTrackData [1][31].Init ( -16617, 243, 273, 1125, 44 );

    m_OriginalTrainTrackData [1][32].Init ( -16457, 243, 273, 1185, 49 );

    m_OriginalTrainTrackData [1][33].Init ( -16297, 243, 273, 1245, 52 );

    m_OriginalTrainTrackData [1][34].Init ( -16137, 230, 254, 1305, 46 );

    m_OriginalTrainTrackData [1][35].Init ( -16109, 228, 254, 1316, 51 );

    m_OriginalTrainTrackData [1][36].Init ( -16086, 236, 254, 1325, 57 );

    m_OriginalTrainTrackData [1][37].Init ( -16070, 252, 254, 1334, 52 );

    m_OriginalTrainTrackData [1][38].Init ( -16060, 272, 254, 1342, 100 );

    m_OriginalTrainTrackData [1][39].Init ( -16054, 288, 254, 1348, 32 );

    m_OriginalTrainTrackData [1][40].Init ( -16053, 307, 254, 1356, 32 );

    m_OriginalTrainTrackData [1][41].Init ( -16052, 467, 231, 1416, 48 );

    m_OriginalTrainTrackData [1][42].Init ( -16052, 627, 212, 1476, 45 );

    m_OriginalTrainTrackData [1][43].Init ( -16052, 787, 212, 1536, 52 );

    m_OriginalTrainTrackData [1][44].Init ( -16052, 947, 212, 1596, 55 );

    m_OriginalTrainTrackData [1][45].Init ( -16052, 1107, 212, 1656, 104 );

    m_OriginalTrainTrackData [1][46].Init ( -16052, 1267, 212, 1716, 49 );

    m_OriginalTrainTrackData [1][47].Init ( -16052, 1427, 212, 1776, 56 );

    m_OriginalTrainTrackData [1][48].Init ( -16052, 1747, 212, 1896, 44 );

    m_OriginalTrainTrackData [1][49].Init ( -16052, 1907, 223, 1956, 32 );

    m_OriginalTrainTrackData [1][50].Init ( -16039, 2067, 237, 2016, 32 );

    m_OriginalTrainTrackData [1][51].Init ( -16033, 2227, 258, 2076, 49 );

    m_OriginalTrainTrackData [1][52].Init ( -16031, 2379, 269, 2133, 114 );

    m_OriginalTrainTrackData [1][53].Init ( -16029, 2482, 272, 2171, 32 );

    m_OriginalTrainTrackData [1][54].Init ( -16029, 2667, 272, 2241, 53 );

    m_OriginalTrainTrackData [1][55].Init ( -16029, 2867, 272, 2316, 44 );

    m_OriginalTrainTrackData [1][56].Init ( -16029, 3187, 272, 2436, 32 );

    m_OriginalTrainTrackData [1][57].Init ( -16029, 3347, 272, 2496, 49 );

    m_OriginalTrainTrackData [1][58].Init ( -16029, 3507, 272, 2556, 49 );

    m_OriginalTrainTrackData [1][59].Init ( -16029, 3667, 272, 2616, 79 );

    m_OriginalTrainTrackData [1][60].Init ( -16029, 3827, 272, 2676, 44 );

    m_OriginalTrainTrackData [1][61].Init ( -16030, 3953, 272, 2723, 45 );

    m_OriginalTrainTrackData [1][62].Init ( -16030, 4113, 272, 2783, 49 );

    m_OriginalTrainTrackData [1][63].Init ( -16030, 4249, 272, 2834, 53 );

    m_OriginalTrainTrackData [1][64].Init ( -16030, 4267, 272, 2841, 50 );

    m_OriginalTrainTrackData [1][65].Init ( -16010, 4427, 272, 2901, 53 );

    m_OriginalTrainTrackData [1][66].Init ( -16009, 4450, 272, 2910, 116 );

    m_OriginalTrainTrackData [1][67].Init ( -16009, 4627, 272, 2976, 49 );

    m_OriginalTrainTrackData [1][68].Init ( -16009, 4746, 272, 3021, 44 );

    m_OriginalTrainTrackData [1][69].Init ( -16008, 4762, 272, 3027, 44 );

    m_OriginalTrainTrackData [1][70].Init ( -16006, 4778, 272, 3033, 32 );

    m_OriginalTrainTrackData [1][71].Init ( -15993, 4799, 272, 3042, 45 );

    m_OriginalTrainTrackData [1][72].Init ( -15969, 4815, 272, 3053, 49 );

    m_OriginalTrainTrackData [1][73].Init ( -15951, 4822, 272, 3060, 111 );

    m_OriginalTrainTrackData [1][74].Init ( -15932, 4826, 272, 3068, 32 );

    m_OriginalTrainTrackData [1][75].Init ( -14464, 4826, 272, 3618, 56 );

    m_OriginalTrainTrackData [1][76].Init ( -14442, 4826, 271, 3626, 50 );

    m_OriginalTrainTrackData [1][77].Init ( -14423, 4826, 270, 3633, 44 );

    m_OriginalTrainTrackData [1][78].Init ( -14133, 4826, 216, 3742, 44 );

    m_OriginalTrainTrackData [1][79].Init ( -14000, 4826, 192, 3792, 32 );

    m_OriginalTrainTrackData [1][80].Init ( -13984, 4826, 190, 3798, 71 );

    m_OriginalTrainTrackData [1][81].Init ( -13969, 4827, 189, 3804, 48 );

    m_OriginalTrainTrackData [1][82].Init ( -13863, 4850, 189, 3844, 46 );

    m_OriginalTrainTrackData [1][83].Init ( -13776, 4905, 189, 3883, 46 );

    m_OriginalTrainTrackData [1][84].Init ( -13715, 4984, 189, 3920, 53 );

    m_OriginalTrainTrackData [1][85].Init ( -13692, 5087, 189, 3960, 55 );

    m_OriginalTrainTrackData [1][86].Init ( -13693, 5750, 189, 4209, 49 );

    m_OriginalTrainTrackData [1][87].Init ( -13688, 5781, 189, 4220, 13 );

    m_OriginalTrainTrackData [1][88].Init ( -13676, 5803, 189, 4230, 32 );

    m_OriginalTrainTrackData [1][89].Init ( -13653, 5819, 189, 4240, 55 );

    m_OriginalTrainTrackData [1][90].Init ( -13613, 5831, 189, 4256, 51 );

    m_OriginalTrainTrackData [1][91].Init ( -13590, 5830, 187, 4265, 54 );

    m_OriginalTrainTrackData [1][92].Init ( -13166, 5830, 111, 4424, 44 );

    m_OriginalTrainTrackData [1][93].Init ( -13142, 5830, 108, 4433, 44 );

    m_OriginalTrainTrackData [1][94].Init ( -13120, 5830, 107, 4441, 10 );

    m_OriginalTrainTrackData [1][95].Init ( -12989, 5830, 107, 4490, 53 );

    m_OriginalTrainTrackData [1][96].Init ( -12967, 5830, 108, 4498, 52 );

    m_OriginalTrainTrackData [1][97].Init ( -12945, 5830, 105, 4506, 50 );

    m_OriginalTrainTrackData [1][98].Init ( -12653, 5830, 52, 4616, 44 );

    m_OriginalTrainTrackData [1][99].Init ( -12571, 5830, 48, 4647, 32 );

    m_OriginalTrainTrackData [1][100].Init ( -12443, 5830, 48, 4695, 50 );

    m_OriginalTrainTrackData [1][101].Init ( -12411, 5836, 48, 4707, 97 );

    m_OriginalTrainTrackData [1][102].Init ( -12386, 5852, 48, 4718, 44 );

    m_OriginalTrainTrackData [1][103].Init ( -12368, 5879, 48, 4730, 50 );

    m_OriginalTrainTrackData [1][104].Init ( -12352, 5950, 48, 4758, 52 );

    m_OriginalTrainTrackData [1][105].Init ( -12280, 6669, 48, 5028, 54 );

    m_OriginalTrainTrackData [1][106].Init ( -12293, 6717, 48, 5047, 44 );

    m_OriginalTrainTrackData [1][107].Init ( -12314, 6750, 48, 5062, 32 );

    m_OriginalTrainTrackData [1][108].Init ( -12323, 6757, 48, 5066, 117 );

    m_OriginalTrainTrackData [1][109].Init ( -12346, 6774, 48, 5077, 48 );

    m_OriginalTrainTrackData [1][110].Init ( -12385, 6786, 48, 5092, 46 );

    m_OriginalTrainTrackData [1][111].Init ( -12413, 6792, 48, 5103, 48 );

    m_OriginalTrainTrackData [1][112].Init ( -12559, 6792, 48, 5158, 56 );

    m_OriginalTrainTrackData [1][113].Init ( -12719, 6792, 52, 5218, 44 );

    m_OriginalTrainTrackData [1][114].Init ( -12879, 6791, 52, 5278, 32 );

    m_OriginalTrainTrackData [1][115].Init ( -13017, 6791, 52, 5329, 118 );

    m_OriginalTrainTrackData [1][116].Init ( -13042, 6791, 54, 5339, 54 );

    m_OriginalTrainTrackData [1][117].Init ( -13073, 6791, 64, 5350, 53 );

    m_OriginalTrainTrackData [1][118].Init ( -13351, 6791, 179, 5455, 44 );

    m_OriginalTrainTrackData [1][119].Init ( -13367, 6791, 185, 5461, 32 );

    m_OriginalTrainTrackData [1][120].Init ( -13388, 6791, 190, 5468, 50 );

    m_OriginalTrainTrackData [1][121].Init ( -13408, 6791, 190, 5476, 46 );

    m_OriginalTrainTrackData [1][122].Init ( -14439, 6791, 189, 5863, 116 );

    m_OriginalTrainTrackData [1][123].Init ( -14465, 6791, 191, 5872, 49 );

    m_OriginalTrainTrackData [1][124].Init ( -14492, 6791, 197, 5882, 56 );

    m_OriginalTrainTrackData [1][125].Init ( -14763, 6791, 266, 5984, 44 );

    m_OriginalTrainTrackData [1][126].Init ( -14778, 6791, 270, 5990, 44 );

    m_OriginalTrainTrackData [1][127].Init ( -14791, 6791, 272, 5995, 44 );

    m_OriginalTrainTrackData [1][128].Init ( -14807, 6791, 272, 6001, 32 );

    m_OriginalTrainTrackData [1][129].Init ( -15284, 6791, 272, 6179, 118 );

    m_OriginalTrainTrackData [1][130].Init ( -15303, 6791, 272, 6187, 53 );

    m_OriginalTrainTrackData [1][131].Init ( -15322, 6791, 274, 6194, 55 );

    m_OriginalTrainTrackData [1][132].Init ( -15740, 6791, 350, 6350, 53 );

    m_OriginalTrainTrackData [1][133].Init ( -15752, 6791, 352, 6355, 55 );

    m_OriginalTrainTrackData [1][134].Init ( -15767, 6791, 354, 6361, 50 );

    m_OriginalTrainTrackData [1][135].Init ( -15783, 6791, 354, 6367, 52 );

    m_OriginalTrainTrackData [1][136].Init ( -15933, 6791, 354, 6423, 13 );

    m_OriginalTrainTrackData [1][137].Init ( -15962, 6803, 354, 6435, 32 );

    m_OriginalTrainTrackData [1][138].Init ( -15991, 6827, 354, 6449, 52 );

    m_OriginalTrainTrackData [1][139].Init ( -16005, 6858, 354, 6461, 56 );

    m_OriginalTrainTrackData [1][140].Init ( -16013, 6899, 354, 6477, 49 );

    m_OriginalTrainTrackData [1][141].Init ( -16013, 7247, 354, 6608, 56 );

    m_OriginalTrainTrackData [1][142].Init ( -16007, 7290, 354, 6624, 52 );

    m_OriginalTrainTrackData [1][143].Init ( -15991, 7328, 354, 6639, 48 );

    m_OriginalTrainTrackData [1][144].Init ( -15964, 7356, 354, 6654, 97 );

    m_OriginalTrainTrackData [1][145].Init ( -15933, 7368, 354, 6666, 45 );

    m_OriginalTrainTrackData [1][146].Init ( -15783, 7368, 354, 6723, 52 );

    m_OriginalTrainTrackData [1][147].Init ( -15758, 7368, 353, 6732, 48 );

    m_OriginalTrainTrackData [1][148].Init ( -15739, 7368, 350, 6739, 50 );

    m_OriginalTrainTrackData [1][149].Init ( -15326, 7368, 275, 6894, 52 );

    m_OriginalTrainTrackData [1][150].Init ( -15303, 7368, 272, 6903, 55 );

    m_OriginalTrainTrackData [1][151].Init ( -15283, 7368, 272, 6910, 107 );

    m_OriginalTrainTrackData [1][152].Init ( -14842, 7369, 272, 7076, 45 );

    m_OriginalTrainTrackData [1][153].Init ( -14826, 7369, 272, 7082, 45 );

    m_OriginalTrainTrackData [1][154].Init ( -14811, 7369, 270, 7087, 51 );

    m_OriginalTrainTrackData [1][155].Init ( -14791, 7369, 264, 7095, 50 );

    m_OriginalTrainTrackData [1][156].Init ( -14582, 7369, 199, 7173, 50 );

    m_OriginalTrainTrackData [1][157].Init ( -14562, 7369, 194, 7181, 53 );

    m_OriginalTrainTrackData [1][158].Init ( -14541, 7369, 190, 7188, 97 );

    m_OriginalTrainTrackData [1][159].Init ( -14524, 7369, 189, 7195, 49 );

    m_OriginalTrainTrackData [1][160].Init ( -13327, 7369, 189, 7644, 50 );

    m_OriginalTrainTrackData [1][161].Init ( -13305, 7369, 189, 7652, 57 );

    m_OriginalTrainTrackData [1][162].Init ( -13281, 7369, 185, 7661, 44 );

    m_OriginalTrainTrackData [1][163].Init ( -13259, 7369, 178, 7669, 44 );

    m_OriginalTrainTrackData [1][164].Init ( -12973, 7369, 59, 7776, 44 );

    m_OriginalTrainTrackData [1][165].Init ( -12951, 7369, 54, 7785, 10 );

    m_OriginalTrainTrackData [1][166].Init ( -12930, 7369, 52, 7793, 44 );

    m_OriginalTrainTrackData [1][167].Init ( -12628, 7370, 52, 7906, 49 );

    m_OriginalTrainTrackData [1][168].Init ( -12573, 7370, 48, 7926, 50 );

    m_OriginalTrainTrackData [1][169].Init ( -12289, 7370, 48, 8033, 44 );

    m_OriginalTrainTrackData [1][170].Init ( -12272, 7376, 48, 8040, 32 );

    m_OriginalTrainTrackData [1][171].Init ( -12253, 7386, 48, 8048, 49 );

    m_OriginalTrainTrackData [1][172].Init ( -12238, 7405, 48, 8057, 104 );

    m_OriginalTrainTrackData [1][173].Init ( -12230, 7431, 48, 8067, 49 );

    m_OriginalTrainTrackData [1][174].Init ( -12226, 7458, 48, 8077, 44 );

    m_OriginalTrainTrackData [1][175].Init ( -12226, 7550, 48, 8112, 32 );

    m_OriginalTrainTrackData [1][176].Init ( -12247, 7606, 48, 8134, 49 );

    m_OriginalTrainTrackData [1][177].Init ( -12286, 7694, 48, 8170, 49 );

    m_OriginalTrainTrackData [1][178].Init ( -12325, 7769, 48, 8202, 46 );

    m_OriginalTrainTrackData [1][179].Init ( -12441, 7925, 48, 8275, 115 );

    m_OriginalTrainTrackData [1][180].Init ( -12506, 7991, 48, 8310, 49 );

    m_OriginalTrainTrackData [1][181].Init ( -12591, 8097, 48, 8361, 48 );

    m_OriginalTrainTrackData [1][182].Init ( -12648, 8186, 48, 8400, 54 );

    m_OriginalTrainTrackData [1][183].Init ( -12673, 8280, 48, 8437, 54 );

    m_OriginalTrainTrackData [1][184].Init ( -12673, 9240, 48, 8797, 46 );

    m_OriginalTrainTrackData [1][185].Init ( -12689, 9378, 48, 8849, 56 );

    m_OriginalTrainTrackData [1][186].Init ( -12711, 9485, 48, 8890, 32 );

    m_OriginalTrainTrackData [1][187].Init ( -12756, 9587, 48, 8932, 54 );

    m_OriginalTrainTrackData [1][188].Init ( -12811, 9691, 48, 8976, 46 );

    m_OriginalTrainTrackData [1][189].Init ( -12889, 9792, 48, 9024, 56 );

    m_OriginalTrainTrackData [1][190].Init ( -13792, 10698, 48, 9503, 54 );

    m_OriginalTrainTrackData [1][191].Init ( -13906, 10811, 48, 9563, 44 );

    m_OriginalTrainTrackData [1][192].Init ( -14116, 10951, 48, 9658, 10 );

    m_OriginalTrainTrackData [1][193].Init ( -14292, 11024, 48, 9729, 54 );

    m_OriginalTrainTrackData [1][194].Init ( -14480, 11047, 48, 9801, 46 );

    m_OriginalTrainTrackData [1][195].Init ( -14669, 11023, 48, 9872, 46 );

    m_OriginalTrainTrackData [1][196].Init ( -14846, 10950, 48, 9944, 53 );

    m_OriginalTrainTrackData [1][197].Init ( -14997, 10833, 48, 10015, 53 );

    m_OriginalTrainTrackData [1][198].Init ( -15111, 10721, 48, 10075, 53 );

    m_OriginalTrainTrackData [1][199].Init ( -15223, 10607, 48, 10135, 13 );

    m_OriginalTrainTrackData [1][200].Init ( -15325, 10528, 48, 10184, 53 );

    m_OriginalTrainTrackData [1][201].Init ( -15444, 10481, 48, 10232, 53 );

    m_OriginalTrainTrackData [1][202].Init ( -15571, 10463, 48, 10280, 49 );

    m_OriginalTrainTrackData [1][203].Init ( -16051, 10463, 48, 10460, 54 );

    m_OriginalTrainTrackData [1][204].Init ( -16188, 10468, 48, 10511, 51 );

    m_OriginalTrainTrackData [1][205].Init ( -16282, 10472, 49, 10546, 44 );

    m_OriginalTrainTrackData [1][206].Init ( -16302, 10472, 49, 10554, 65 );

    m_OriginalTrainTrackData [1][207].Init ( -16322, 10468, 49, 10562, 44 );

    m_OriginalTrainTrackData [1][208].Init ( -16341, 10456, 49, 10570, 49 );

    m_OriginalTrainTrackData [1][209].Init ( -16359, 10438, 49, 10580, 51 );

    m_OriginalTrainTrackData [1][210].Init ( -16439, 10312, 49, 10636, 57 );

    m_OriginalTrainTrackData [1][211].Init ( -16491, 10243, 53, 10668, 46 );

    m_OriginalTrainTrackData [1][212].Init ( -16581, 10199, 73, 10705, 51 );

    m_OriginalTrainTrackData [1][213].Init ( -16684, 10194, 99, 10744, 44 );

    m_OriginalTrainTrackData [1][214].Init ( -16980, 10194, 153, 10855, 50 );

    m_OriginalTrainTrackData [1][215].Init ( -17265, 10194, 205, 10962, 52 );

    m_OriginalTrainTrackData [1][216].Init ( -17437, 10193, 228, 11027, 54 );

    m_OriginalTrainTrackData [1][217].Init ( -17785, 10194, 290, 11157, 46 );

    m_OriginalTrainTrackData [1][218].Init ( -17900, 10194, 309, 11200, 55 );

    m_OriginalTrainTrackData [1][219].Init ( -18012, 10169, 322, 11243, 57 );

    m_OriginalTrainTrackData [1][220].Init ( -18117, 10099, 333, 11291, 44 );

    m_OriginalTrainTrackData [1][221].Init ( -18173, 9997, 344, 11334, 55 );

    m_OriginalTrainTrackData [1][222].Init ( -18187, 9900, 358, 11371, 44 );

    m_OriginalTrainTrackData [1][223].Init ( -18178, 9763, 383, 11422, 32 );

    m_OriginalTrainTrackData [1][224].Init ( -18135, 9603, 428, 11485, 51 );

    m_OriginalTrainTrackData [1][225].Init ( -18118, 9525, 436, 11514, 48 );

    m_OriginalTrainTrackData [1][226].Init ( -18119, 9320, 438, 11591, 50 );

    m_OriginalTrainTrackData [1][227].Init ( -18119, 9295, 441, 11601, 50 );

    m_OriginalTrainTrackData [1][228].Init ( -18118, 9270, 449, 11610, 48 );

    m_OriginalTrainTrackData [1][229].Init ( -18119, 8868, 617, 11761, 55 );

    m_OriginalTrainTrackData [1][230].Init ( -18119, 8847, 626, 11769, 46 );

    m_OriginalTrainTrackData [1][231].Init ( -18119, 8827, 630, 11776, 56 );

    m_OriginalTrainTrackData [1][232].Init ( -18119, 8807, 630, 11784, 56 );

    m_OriginalTrainTrackData [1][233].Init ( -18119, 8667, 630, 11836, 51 );

    m_OriginalTrainTrackData [1][234].Init ( -18119, 8387, 661, 11941, 52 );

    m_OriginalTrainTrackData [1][235].Init ( -18119, 8227, 661, 12001, 50 );

    m_OriginalTrainTrackData [1][236].Init ( -18119, 8106, 662, 12047, 50 );

    m_OriginalTrainTrackData [1][237].Init ( -18119, 8081, 660, 12056, 44 );

    m_OriginalTrainTrackData [1][238].Init ( -18119, 8061, 653, 12063, 32 );

    m_OriginalTrainTrackData [1][239].Init ( -18119, 7785, 538, 12167, 71 );

    m_OriginalTrainTrackData [1][240].Init ( -18119, 7764, 532, 12175, 44 );

    m_OriginalTrainTrackData [1][241].Init ( -18119, 7746, 528, 12182, 32 );

    m_OriginalTrainTrackData [1][242].Init ( -18119, 7668, 524, 12211, 57 );

    m_OriginalTrainTrackData [1][243].Init ( -18119, 6910, 523, 12495, 50 );

    m_OriginalTrainTrackData [1][244].Init ( -18119, 6896, 523, 12500, 46 );

    m_OriginalTrainTrackData [1][245].Init ( -18119, 6881, 520, 12506, 46 );

    m_OriginalTrainTrackData [1][246].Init ( -18119, 6866, 516, 12512, 50 );

    m_OriginalTrainTrackData [1][247].Init ( -18119, 6854, 512, 12516, 46 );

    m_OriginalTrainTrackData [1][248].Init ( -18119, 6578, 397, 12620, 52 );

    m_OriginalTrainTrackData [1][249].Init ( -18119, 6558, 390, 12627, 50 );

    m_OriginalTrainTrackData [1][250].Init ( -18119, 6543, 387, 12633, 44 );

    m_OriginalTrainTrackData [1][251].Init ( -18119, 6527, 387, 12639, 32 );

    m_OriginalTrainTrackData [1][252].Init ( -18120, 5427, 386, 13051, 71 );

    m_OriginalTrainTrackData [1][253].Init ( -18119, 5139, 386, 13159, 44 );

    m_OriginalTrainTrackData [1][254].Init ( -18119, 5109, 386, 13170, 32 );

    m_OriginalTrainTrackData [1][255].Init ( -18119, 5080, 382, 13181, 57 );

    m_OriginalTrainTrackData [1][256].Init ( -18119, 4650, 275, 13343, 53 );

    m_OriginalTrainTrackData [1][257].Init ( -18119, 4586, 272, 13367, 51 );

    m_OriginalTrainTrackData [1][258].Init ( -18119, 4527, 272, 13389, 55 );

    m_OriginalTrainTrackData [1][259].Init ( -18119, 4427, 272, 13426, 10 );

    m_OriginalTrainTrackData [1][260].Init ( -18117, 4147, 273, 13531, 13 );

    m_OriginalTrainTrackData [1][261].Init ( -18130, 4103, 272, 13548, 57 );

    m_OriginalTrainTrackData [1][262].Init ( -18179, 4072, 272, 13570, 48 );

    m_OriginalTrainTrackData [1][263].Init ( -18213, 4070, 272, 13583, 55 );

    m_OriginalTrainTrackData [1][264].Init ( -18373, 4068, 272, 13643, 50 );

    m_OriginalTrainTrackData [1][265].Init ( -18396, 4068, 270, 13652, 115 );

    m_OriginalTrainTrackData [1][266].Init ( -18533, 4068, 251, 13703, 44 );

    m_OriginalTrainTrackData [1][267].Init ( -18692, 4068, 230, 13763, 32 );

    m_OriginalTrainTrackData [1][268].Init ( -18824, 4068, 229, 13812, 49 );

    m_OriginalTrainTrackData [1][269].Init ( -18850, 4068, 228, 13822, 53 );

    m_OriginalTrainTrackData [1][270].Init ( -18871, 4066, 228, 13830, 49 );

    m_OriginalTrainTrackData [1][271].Init ( -18907, 4062, 228, 13843, 56 );

    m_OriginalTrainTrackData [1][272].Init ( -18928, 4053, 228, 13852, 109 );

    m_OriginalTrainTrackData [1][273].Init ( -18946, 4043, 228, 13860, 32 );

    m_OriginalTrainTrackData [1][274].Init ( -18956, 4023, 229, 13868, 32 );

    m_OriginalTrainTrackData [1][275].Init ( -18960, 4000, 229, 13877, 49 );

    m_OriginalTrainTrackData [1][276].Init ( -18943, 3961, 233, 13893, 50 );

    m_OriginalTrainTrackData [1][277].Init ( -18923, 3920, 237, 13910, 52 );

    m_OriginalTrainTrackData [1][278].Init ( -18838, 3783, 237, 13970, 51 );

    m_OriginalTrainTrackData [1][279].Init ( -18743, 3656, 253, 14030, 109 );

    m_OriginalTrainTrackData [1][280].Init ( -18640, 3533, 266, 14090, 32 );

    m_OriginalTrainTrackData [1][281].Init ( -18531, 3416, 272, 14150, 32 );

    m_OriginalTrainTrackData [1][282].Init ( -18417, 3303, 272, 14210, 49 );

    m_OriginalTrainTrackData [1][283].Init ( -18304, 3190, 272, 14270, 50 );

    m_OriginalTrainTrackData [1][284].Init ( -18191, 3077, 267, 14330, 52 );

    m_OriginalTrainTrackData [1][285].Init ( -18093, 2951, 258, 14390, 57 );

    m_OriginalTrainTrackData [1][286].Init ( -18032, 2801, 255, 14450, 115 );

    m_OriginalTrainTrackData [1][287].Init ( -18011, 2642, 273, 14511, 49 );

    m_OriginalTrainTrackData [1][288].Init ( -18012, 2482, 273, 14571, 44 );

    m_OriginalTrainTrackData [1][289].Init ( -18012, 2242, 273, 14661, 32 );

    m_OriginalTrainTrackData [1][290].Init ( -18011, 1922, 273, 14781, 50 );

    m_OriginalTrainTrackData [1][291].Init ( -18011, 1746, 273, 14847, 49 );

    m_OriginalTrainTrackData [1][292].Init ( -18012, 1586, 273, 14907, 48 );

    m_OriginalTrainTrackData [1][293].Init ( -18012, 1426, 273, 14967, 115 );

    m_OriginalTrainTrackData [1][294].Init ( -18012, 1266, 273, 15027, 32 );

    m_OriginalTrainTrackData [1][295].Init ( -18013, 1106, 273, 15087, 50 );

    m_OriginalTrainTrackData [1][296].Init ( -18014, 946, 273, 15147, 56 );

    m_OriginalTrainTrackData [1][297].Init ( -18016, 786, 273, 15207, 44 );

    m_OriginalTrainTrackData [1][298].Init ( -18022, 626, 273, 15267, 32 );

    m_OriginalTrainTrackData [1][299].Init ( -18029, 477, 273, 15323, 32 );

    m_OriginalTrainTrackData [1][0].Init ( -18029, 317, 273, 0, 32 );

    m_OriginalTrainTrackData [1][1].Init ( -18035, 157, 273, 60, 54 );

    m_OriginalTrainTrackData [1][2].Init ( -18039, -2, 273, 119, 53 );

    m_OriginalTrainTrackData [1][3].Init ( -18044, -162, 273, 179, 32 );

    m_OriginalTrainTrackData [1][4].Init ( -18044, -322, 273, 239, 54 );

    m_OriginalTrainTrackData [1][5].Init ( -18044, -482, 273, 299, 52 );

    m_OriginalTrainTrackData [1][6].Init ( -18035, -506, 273, 309, 52 );

    m_OriginalTrainTrackData [1][7].Init ( -18034, -509, 273, 310, 46 );

    m_OriginalTrainTrackData [1][8].Init ( -18021, -530, 273, 319, 46 );

    m_OriginalTrainTrackData [1][9].Init ( -17995, -548, 273, 331, 54 );

    m_OriginalTrainTrackData [1][10].Init ( -17975, -559, 273, 340, 44 );

    m_OriginalTrainTrackData [1][11].Init ( -17950, -561, 273, 349, 52 );

    m_OriginalTrainTrackData [1][12].Init ( -17897, -561, 273, 369, 54 );

    m_OriginalTrainTrackData [1][13].Init ( -17577, -560, 273, 489, 57 );

    m_OriginalTrainTrackData [1][14].Init ( -17399, -560, 273, 556, 46 );

    m_OriginalTrainTrackData [1][15].Init ( -17374, -553, 273, 565, 49 );

    m_OriginalTrainTrackData [1][16].Init ( -17359, -542, 273, 572, 46 );

    m_OriginalTrainTrackData [1][17].Init ( -17345, -527, 273, 580, 83 );

    m_OriginalTrainTrackData [1][18].Init ( -17336, -509, 273, 588, 32 );

    m_OriginalTrainTrackData [1][19].Init ( -17333, -482, 273, 598, 49 );

    m_OriginalTrainTrackData [1][20].Init ( -17333, -317, 273, 660, 57 );

    m_OriginalTrainTrackData [1][21].Init ( -17333, -157, 273, 720, 51 );

    m_OriginalTrainTrackData [1][22].Init ( -17333, 2, 273, 779, 55 );

    m_OriginalTrainTrackData [1][23].Init ( -17332, 162, 273, 839, 46 );

    m_OriginalTrainTrackData [1][24].Init ( -17326, 189, 273, 850, 83 );

    m_OriginalTrainTrackData [1][25].Init ( -17312, 217, 273, 861, 32 );

    m_OriginalTrainTrackData [1][26].Init ( -17290, 237, 273, 873, 49 );

    m_OriginalTrainTrackData [1][27].Init ( -17257, 244, 273, 885, 48 );

    m_OriginalTrainTrackData [1][28].Init ( -17097, 243, 273, 945, 57 );

    m_OriginalTrainTrackData [1][29].Init ( -16937, 243, 273, 1005, 46 );

    m_OriginalTrainTrackData [1][30].Init ( -16777, 243, 273, 1065, 54 );

    m_OriginalTrainTrackData [1][31].Init ( -16617, 243, 273, 1125, 44 );

    m_OriginalTrainTrackData [1][32].Init ( -16457, 243, 273, 1185, 49 );

    m_OriginalTrainTrackData [1][33].Init ( -16297, 243, 273, 1245, 52 );

    m_OriginalTrainTrackData [1][34].Init ( -16137, 230, 254, 1305, 46 );

    m_OriginalTrainTrackData [1][35].Init ( -16109, 228, 254, 1316, 51 );

    m_OriginalTrainTrackData [1][36].Init ( -16086, 236, 254, 1325, 57 );

    m_OriginalTrainTrackData [1][37].Init ( -16070, 252, 254, 1334, 52 );

    m_OriginalTrainTrackData [1][38].Init ( -16060, 272, 254, 1342, 100 );

    m_OriginalTrainTrackData [1][39].Init ( -16054, 288, 254, 1348, 32 );

    m_OriginalTrainTrackData [1][40].Init ( -16053, 307, 254, 1356, 32 );

    m_OriginalTrainTrackData [1][41].Init ( -16052, 467, 231, 1416, 48 );

    m_OriginalTrainTrackData [1][42].Init ( -16052, 627, 212, 1476, 45 );

    m_OriginalTrainTrackData [1][43].Init ( -16052, 787, 212, 1536, 52 );

    m_OriginalTrainTrackData [1][44].Init ( -16052, 947, 212, 1596, 55 );

    m_OriginalTrainTrackData [1][45].Init ( -16052, 1107, 212, 1656, 104 );

    m_OriginalTrainTrackData [1][46].Init ( -16052, 1267, 212, 1716, 49 );

    m_OriginalTrainTrackData [1][47].Init ( -16052, 1427, 212, 1776, 56 );

    m_OriginalTrainTrackData [1][48].Init ( -16052, 1747, 212, 1896, 44 );

    m_OriginalTrainTrackData [1][49].Init ( -16052, 1907, 223, 1956, 32 );

    m_OriginalTrainTrackData [1][50].Init ( -16039, 2067, 237, 2016, 32 );

    m_OriginalTrainTrackData [1][51].Init ( -16033, 2227, 258, 2076, 49 );

    m_OriginalTrainTrackData [1][52].Init ( -16031, 2379, 269, 2133, 114 );

    m_OriginalTrainTrackData [1][53].Init ( -16029, 2482, 272, 2171, 32 );

    m_OriginalTrainTrackData [1][54].Init ( -16029, 2667, 272, 2241, 53 );

    m_OriginalTrainTrackData [1][55].Init ( -16029, 2867, 272, 2316, 44 );

    m_OriginalTrainTrackData [1][56].Init ( -16029, 3187, 272, 2436, 32 );

    m_OriginalTrainTrackData [1][57].Init ( -16029, 3347, 272, 2496, 49 );

    m_OriginalTrainTrackData [1][58].Init ( -16029, 3507, 272, 2556, 49 );

    m_OriginalTrainTrackData [1][59].Init ( -16029, 3667, 272, 2616, 79 );

    m_OriginalTrainTrackData [1][60].Init ( -16029, 3827, 272, 2676, 44 );

    m_OriginalTrainTrackData [1][61].Init ( -16030, 3953, 272, 2723, 45 );

    m_OriginalTrainTrackData [1][62].Init ( -16030, 4113, 272, 2783, 49 );

    m_OriginalTrainTrackData [1][63].Init ( -16030, 4249, 272, 2834, 53 );

    m_OriginalTrainTrackData [1][64].Init ( -16030, 4267, 272, 2841, 50 );

    m_OriginalTrainTrackData [1][65].Init ( -16010, 4427, 272, 2901, 53 );

    m_OriginalTrainTrackData [1][66].Init ( -16009, 4450, 272, 2910, 116 );

    m_OriginalTrainTrackData [1][67].Init ( -16009, 4627, 272, 2976, 49 );

    m_OriginalTrainTrackData [1][68].Init ( -16009, 4746, 272, 3021, 44 );

    m_OriginalTrainTrackData [1][69].Init ( -16008, 4762, 272, 3027, 44 );

    m_OriginalTrainTrackData [1][70].Init ( -16006, 4778, 272, 3033, 32 );

    m_OriginalTrainTrackData [1][71].Init ( -15993, 4799, 272, 3042, 45 );

    m_OriginalTrainTrackData [1][72].Init ( -15969, 4815, 272, 3053, 49 );

    m_OriginalTrainTrackData [1][73].Init ( -15951, 4822, 272, 3060, 111 );

    m_OriginalTrainTrackData [1][74].Init ( -15932, 4826, 272, 3068, 32 );

    m_OriginalTrainTrackData [1][75].Init ( -14464, 4826, 272, 3618, 56 );

    m_OriginalTrainTrackData [1][76].Init ( -14442, 4826, 271, 3626, 50 );

    m_OriginalTrainTrackData [1][77].Init ( -14423, 4826, 270, 3633, 44 );

    m_OriginalTrainTrackData [1][78].Init ( -14133, 4826, 216, 3742, 44 );

    m_OriginalTrainTrackData [1][79].Init ( -14000, 4826, 192, 3792, 32 );

    m_OriginalTrainTrackData [1][80].Init ( -13984, 4826, 190, 3798, 71 );

    m_OriginalTrainTrackData [1][81].Init ( -13969, 4827, 189, 3804, 48 );

    m_OriginalTrainTrackData [1][82].Init ( -13863, 4850, 189, 3844, 46 );

    m_OriginalTrainTrackData [1][83].Init ( -13776, 4905, 189, 3883, 46 );

    m_OriginalTrainTrackData [1][84].Init ( -13715, 4984, 189, 3920, 53 );

    m_OriginalTrainTrackData [1][85].Init ( -13692, 5087, 189, 3960, 55 );

    m_OriginalTrainTrackData [1][86].Init ( -13693, 5750, 189, 4209, 49 );

    m_OriginalTrainTrackData [1][87].Init ( -13688, 5781, 189, 4220, 13 );

    m_OriginalTrainTrackData [1][88].Init ( -13676, 5803, 189, 4230, 32 );

    m_OriginalTrainTrackData [1][89].Init ( -13653, 5819, 189, 4240, 55 );

    m_OriginalTrainTrackData [1][90].Init ( -13613, 5831, 189, 4256, 51 );

    m_OriginalTrainTrackData [1][91].Init ( -13590, 5830, 187, 4265, 54 );

    m_OriginalTrainTrackData [1][92].Init ( -13166, 5830, 111, 4424, 44 );

    m_OriginalTrainTrackData [1][93].Init ( -13142, 5830, 108, 4433, 44 );

    m_OriginalTrainTrackData [1][94].Init ( -13120, 5830, 107, 4441, 10 );

    m_OriginalTrainTrackData [1][95].Init ( -12989, 5830, 107, 4490, 53 );

    m_OriginalTrainTrackData [1][96].Init ( -12967, 5830, 108, 4498, 52 );

    m_OriginalTrainTrackData [1][97].Init ( -12945, 5830, 105, 4506, 50 );

    m_OriginalTrainTrackData [1][98].Init ( -12653, 5830, 52, 4616, 44 );

    m_OriginalTrainTrackData [1][99].Init ( -12571, 5830, 48, 4647, 32 );

    m_OriginalTrainTrackData [1][100].Init ( -12443, 5830, 48, 4695, 50 );

    m_OriginalTrainTrackData [1][101].Init ( -12411, 5836, 48, 4707, 97 );

    m_OriginalTrainTrackData [1][102].Init ( -12386, 5852, 48, 4718, 44 );

    m_OriginalTrainTrackData [1][103].Init ( -12368, 5879, 48, 4730, 50 );

    m_OriginalTrainTrackData [1][104].Init ( -12352, 5950, 48, 4758, 52 );

    m_OriginalTrainTrackData [1][105].Init ( -12280, 6669, 48, 5028, 54 );

    m_OriginalTrainTrackData [1][106].Init ( -12293, 6717, 48, 5047, 44 );

    m_OriginalTrainTrackData [1][107].Init ( -12314, 6750, 48, 5062, 32 );

    m_OriginalTrainTrackData [1][108].Init ( -12323, 6757, 48, 5066, 117 );

    m_OriginalTrainTrackData [1][109].Init ( -12346, 6774, 48, 5077, 48 );

    m_OriginalTrainTrackData [1][110].Init ( -12385, 6786, 48, 5092, 46 );

    m_OriginalTrainTrackData [1][111].Init ( -12413, 6792, 48, 5103, 48 );

    m_OriginalTrainTrackData [1][112].Init ( -12559, 6792, 48, 5158, 56 );

    m_OriginalTrainTrackData [1][113].Init ( -12719, 6792, 52, 5218, 44 );

    m_OriginalTrainTrackData [1][114].Init ( -12879, 6791, 52, 5278, 32 );

    m_OriginalTrainTrackData [1][115].Init ( -13017, 6791, 52, 5329, 118 );

    m_OriginalTrainTrackData [1][116].Init ( -13042, 6791, 54, 5339, 54 );

    m_OriginalTrainTrackData [1][117].Init ( -13073, 6791, 64, 5350, 53 );

    m_OriginalTrainTrackData [1][118].Init ( -13351, 6791, 179, 5455, 44 );

    m_OriginalTrainTrackData [1][119].Init ( -13367, 6791, 185, 5461, 32 );

    m_OriginalTrainTrackData [1][120].Init ( -13388, 6791, 190, 5468, 50 );

    m_OriginalTrainTrackData [1][121].Init ( -13408, 6791, 190, 5476, 46 );

    m_OriginalTrainTrackData [1][122].Init ( -14439, 6791, 189, 5863, 116 );

    m_OriginalTrainTrackData [1][123].Init ( -14465, 6791, 191, 5872, 49 );

    m_OriginalTrainTrackData [1][124].Init ( -14492, 6791, 197, 5882, 56 );

    m_OriginalTrainTrackData [1][125].Init ( -14763, 6791, 266, 5984, 44 );

    m_OriginalTrainTrackData [1][126].Init ( -14778, 6791, 270, 5990, 44 );

    m_OriginalTrainTrackData [1][127].Init ( -14791, 6791, 272, 5995, 44 );

    m_OriginalTrainTrackData [1][128].Init ( -14807, 6791, 272, 6001, 32 );

    m_OriginalTrainTrackData [1][129].Init ( -15284, 6791, 272, 6179, 118 );

    m_OriginalTrainTrackData [1][130].Init ( -15303, 6791, 272, 6187, 53 );

    m_OriginalTrainTrackData [1][131].Init ( -15322, 6791, 274, 6194, 55 );

    m_OriginalTrainTrackData [1][132].Init ( -15740, 6791, 350, 6350, 53 );

    m_OriginalTrainTrackData [1][133].Init ( -15752, 6791, 352, 6355, 55 );

    m_OriginalTrainTrackData [1][134].Init ( -15767, 6791, 354, 6361, 50 );

    m_OriginalTrainTrackData [1][135].Init ( -15783, 6791, 354, 6367, 52 );

    m_OriginalTrainTrackData [1][136].Init ( -15933, 6791, 354, 6423, 13 );

    m_OriginalTrainTrackData [1][137].Init ( -15962, 6803, 354, 6435, 32 );

    m_OriginalTrainTrackData [1][138].Init ( -15991, 6827, 354, 6449, 52 );

    m_OriginalTrainTrackData [1][139].Init ( -16005, 6858, 354, 6461, 56 );

    m_OriginalTrainTrackData [1][140].Init ( -16013, 6899, 354, 6477, 49 );

    m_OriginalTrainTrackData [1][141].Init ( -16013, 7247, 354, 6608, 56 );

    m_OriginalTrainTrackData [1][142].Init ( -16007, 7290, 354, 6624, 52 );

    m_OriginalTrainTrackData [1][143].Init ( -15991, 7328, 354, 6639, 48 );

    m_OriginalTrainTrackData [1][144].Init ( -15964, 7356, 354, 6654, 97 );

    m_OriginalTrainTrackData [1][145].Init ( -15933, 7368, 354, 6666, 45 );

    m_OriginalTrainTrackData [1][146].Init ( -15783, 7368, 354, 6723, 52 );

    m_OriginalTrainTrackData [1][147].Init ( -15758, 7368, 353, 6732, 48 );

    m_OriginalTrainTrackData [1][148].Init ( -15739, 7368, 350, 6739, 50 );

    m_OriginalTrainTrackData [1][149].Init ( -15326, 7368, 275, 6894, 52 );

    m_OriginalTrainTrackData [1][150].Init ( -15303, 7368, 272, 6903, 55 );

    m_OriginalTrainTrackData [1][151].Init ( -15283, 7368, 272, 6910, 107 );

    m_OriginalTrainTrackData [1][152].Init ( -14842, 7369, 272, 7076, 45 );

    m_OriginalTrainTrackData [1][153].Init ( -14826, 7369, 272, 7082, 45 );

    m_OriginalTrainTrackData [1][154].Init ( -14811, 7369, 270, 7087, 51 );

    m_OriginalTrainTrackData [1][155].Init ( -14791, 7369, 264, 7095, 50 );

    m_OriginalTrainTrackData [1][156].Init ( -14582, 7369, 199, 7173, 50 );

    m_OriginalTrainTrackData [1][157].Init ( -14562, 7369, 194, 7181, 53 );

    m_OriginalTrainTrackData [1][158].Init ( -14541, 7369, 190, 7188, 97 );

    m_OriginalTrainTrackData [1][159].Init ( -14524, 7369, 189, 7195, 49 );

    m_OriginalTrainTrackData [1][160].Init ( -13327, 7369, 189, 7644, 50 );

    m_OriginalTrainTrackData [1][161].Init ( -13305, 7369, 189, 7652, 57 );

    m_OriginalTrainTrackData [1][162].Init ( -13281, 7369, 185, 7661, 44 );

    m_OriginalTrainTrackData [1][163].Init ( -13259, 7369, 178, 7669, 44 );

    m_OriginalTrainTrackData [1][164].Init ( -12973, 7369, 59, 7776, 44 );

    m_OriginalTrainTrackData [1][165].Init ( -12951, 7369, 54, 7785, 10 );

    m_OriginalTrainTrackData [1][166].Init ( -12930, 7369, 52, 7793, 44 );

    m_OriginalTrainTrackData [1][167].Init ( -12628, 7370, 52, 7906, 49 );

    m_OriginalTrainTrackData [1][168].Init ( -12573, 7370, 48, 7926, 50 );

    m_OriginalTrainTrackData [1][169].Init ( -12289, 7370, 48, 8033, 44 );

    m_OriginalTrainTrackData [1][170].Init ( -12272, 7376, 48, 8040, 32 );

    m_OriginalTrainTrackData [1][171].Init ( -12253, 7386, 48, 8048, 49 );

    m_OriginalTrainTrackData [1][172].Init ( -12238, 7405, 48, 8057, 104 );

    m_OriginalTrainTrackData [1][173].Init ( -12230, 7431, 48, 8067, 49 );

    m_OriginalTrainTrackData [1][174].Init ( -12226, 7458, 48, 8077, 44 );

    m_OriginalTrainTrackData [1][175].Init ( -12226, 7550, 48, 8112, 32 );

    m_OriginalTrainTrackData [1][176].Init ( -12247, 7606, 48, 8134, 49 );

    m_OriginalTrainTrackData [1][177].Init ( -12286, 7694, 48, 8170, 49 );

    m_OriginalTrainTrackData [1][178].Init ( -12325, 7769, 48, 8202, 46 );

    m_OriginalTrainTrackData [1][179].Init ( -12441, 7925, 48, 8275, 115 );

    m_OriginalTrainTrackData [1][180].Init ( -12506, 7991, 48, 8310, 49 );

    m_OriginalTrainTrackData [1][181].Init ( -12591, 8097, 48, 8361, 48 );

    m_OriginalTrainTrackData [1][182].Init ( -12648, 8186, 48, 8400, 54 );

    m_OriginalTrainTrackData [1][183].Init ( -12673, 8280, 48, 8437, 54 );

    m_OriginalTrainTrackData [1][184].Init ( -12673, 9240, 48, 8797, 46 );

    m_OriginalTrainTrackData [1][185].Init ( -12689, 9378, 48, 8849, 56 );

    m_OriginalTrainTrackData [1][186].Init ( -12711, 9485, 48, 8890, 32 );

    m_OriginalTrainTrackData [1][187].Init ( -12756, 9587, 48, 8932, 54 );

    m_OriginalTrainTrackData [1][188].Init ( -12811, 9691, 48, 8976, 46 );

    m_OriginalTrainTrackData [1][189].Init ( -12889, 9792, 48, 9024, 56 );

    m_OriginalTrainTrackData [1][190].Init ( -13792, 10698, 48, 9503, 54 );

    m_OriginalTrainTrackData [1][191].Init ( -13906, 10811, 48, 9563, 44 );

    m_OriginalTrainTrackData [1][192].Init ( -14116, 10951, 48, 9658, 10 );

    m_OriginalTrainTrackData [1][193].Init ( -14292, 11024, 48, 9729, 54 );

    m_OriginalTrainTrackData [1][194].Init ( -14480, 11047, 48, 9801, 46 );

    m_OriginalTrainTrackData [1][195].Init ( -14669, 11023, 48, 9872, 46 );

    m_OriginalTrainTrackData [1][196].Init ( -14846, 10950, 48, 9944, 53 );

    m_OriginalTrainTrackData [1][197].Init ( -14997, 10833, 48, 10015, 53 );

    m_OriginalTrainTrackData [1][198].Init ( -15111, 10721, 48, 10075, 53 );

    m_OriginalTrainTrackData [1][199].Init ( -15223, 10607, 48, 10135, 13 );

    m_OriginalTrainTrackData [1][200].Init ( -15325, 10528, 48, 10184, 53 );

    m_OriginalTrainTrackData [1][201].Init ( -15444, 10481, 48, 10232, 53 );

    m_OriginalTrainTrackData [1][202].Init ( -15571, 10463, 48, 10280, 49 );

    m_OriginalTrainTrackData [1][203].Init ( -16051, 10463, 48, 10460, 54 );

    m_OriginalTrainTrackData [1][204].Init ( -16188, 10468, 48, 10511, 51 );

    m_OriginalTrainTrackData [1][205].Init ( -16282, 10472, 49, 10546, 44 );

    m_OriginalTrainTrackData [1][206].Init ( -16302, 10472, 49, 10554, 65 );

    m_OriginalTrainTrackData [1][207].Init ( -16322, 10468, 49, 10562, 44 );

    m_OriginalTrainTrackData [1][208].Init ( -16341, 10456, 49, 10570, 49 );

    m_OriginalTrainTrackData [1][209].Init ( -16359, 10438, 49, 10580, 51 );

    m_OriginalTrainTrackData [1][210].Init ( -16439, 10312, 49, 10636, 57 );

    m_OriginalTrainTrackData [1][211].Init ( -16491, 10243, 53, 10668, 46 );

    m_OriginalTrainTrackData [1][212].Init ( -16581, 10199, 73, 10705, 51 );

    m_OriginalTrainTrackData [1][213].Init ( -16684, 10194, 99, 10744, 44 );

    m_OriginalTrainTrackData [1][214].Init ( -16980, 10194, 153, 10855, 50 );

    m_OriginalTrainTrackData [1][215].Init ( -17265, 10194, 205, 10962, 52 );

    m_OriginalTrainTrackData [1][216].Init ( -17437, 10193, 228, 11027, 54 );

    m_OriginalTrainTrackData [1][217].Init ( -17785, 10194, 290, 11157, 46 );

    m_OriginalTrainTrackData [1][218].Init ( -17900, 10194, 309, 11200, 55 );

    m_OriginalTrainTrackData [1][219].Init ( -18012, 10169, 322, 11243, 57 );

    m_OriginalTrainTrackData [1][220].Init ( -18117, 10099, 333, 11291, 44 );

    m_OriginalTrainTrackData [1][221].Init ( -18173, 9997, 344, 11334, 55 );

    m_OriginalTrainTrackData [1][222].Init ( -18187, 9900, 358, 11371, 44 );

    m_OriginalTrainTrackData [1][223].Init ( -18178, 9763, 383, 11422, 32 );

    m_OriginalTrainTrackData [1][224].Init ( -18135, 9603, 428, 11485, 51 );

    m_OriginalTrainTrackData [1][225].Init ( -18118, 9525, 436, 11514, 48 );

    m_OriginalTrainTrackData [1][226].Init ( -18119, 9320, 438, 11591, 50 );

    m_OriginalTrainTrackData [1][227].Init ( -18119, 9295, 441, 11601, 50 );

    m_OriginalTrainTrackData [1][228].Init ( -18118, 9270, 449, 11610, 48 );

    m_OriginalTrainTrackData [1][229].Init ( -18119, 8868, 617, 11761, 55 );

    m_OriginalTrainTrackData [1][230].Init ( -18119, 8847, 626, 11769, 46 );

    m_OriginalTrainTrackData [1][231].Init ( -18119, 8827, 630, 11776, 56 );

    m_OriginalTrainTrackData [1][232].Init ( -18119, 8807, 630, 11784, 56 );

    m_OriginalTrainTrackData [1][233].Init ( -18119, 8667, 630, 11836, 51 );

    m_OriginalTrainTrackData [1][234].Init ( -18119, 8387, 661, 11941, 52 );

    m_OriginalTrainTrackData [1][235].Init ( -18119, 8227, 661, 12001, 50 );

    m_OriginalTrainTrackData [1][236].Init ( -18119, 8106, 662, 12047, 50 );

    m_OriginalTrainTrackData [1][237].Init ( -18119, 8081, 660, 12056, 44 );

    m_OriginalTrainTrackData [1][238].Init ( -18119, 8061, 653, 12063, 32 );

    m_OriginalTrainTrackData [1][239].Init ( -18119, 7785, 538, 12167, 71 );

    m_OriginalTrainTrackData [1][240].Init ( -18119, 7764, 532, 12175, 44 );

    m_OriginalTrainTrackData [1][241].Init ( -18119, 7746, 528, 12182, 32 );

    m_OriginalTrainTrackData [1][242].Init ( -18119, 7668, 524, 12211, 57 );

    m_OriginalTrainTrackData [1][243].Init ( -18119, 6910, 523, 12495, 50 );

    m_OriginalTrainTrackData [1][244].Init ( -18119, 6896, 523, 12500, 46 );

    m_OriginalTrainTrackData [1][245].Init ( -18119, 6881, 520, 12506, 46 );

    m_OriginalTrainTrackData [1][246].Init ( -18119, 6866, 516, 12512, 50 );

    m_OriginalTrainTrackData [1][247].Init ( -18119, 6854, 512, 12516, 46 );

    m_OriginalTrainTrackData [1][248].Init ( -18119, 6578, 397, 12620, 52 );

    m_OriginalTrainTrackData [1][249].Init ( -18119, 6558, 390, 12627, 50 );

    m_OriginalTrainTrackData [1][250].Init ( -18119, 6543, 387, 12633, 44 );

    m_OriginalTrainTrackData [1][251].Init ( -18119, 6527, 387, 12639, 32 );

    m_OriginalTrainTrackData [1][252].Init ( -18120, 5427, 386, 13051, 71 );

    m_OriginalTrainTrackData [1][253].Init ( -18119, 5139, 386, 13159, 44 );

    m_OriginalTrainTrackData [1][254].Init ( -18119, 5109, 386, 13170, 32 );

    m_OriginalTrainTrackData [1][255].Init ( -18119, 5080, 382, 13181, 57 );

    m_OriginalTrainTrackData [1][256].Init ( -18119, 4650, 275, 13343, 53 );

    m_OriginalTrainTrackData [1][257].Init ( -18119, 4586, 272, 13367, 51 );

    m_OriginalTrainTrackData [1][258].Init ( -18119, 4527, 272, 13389, 55 );

    m_OriginalTrainTrackData [1][259].Init ( -18119, 4427, 272, 13426, 10 );

    m_OriginalTrainTrackData [1][260].Init ( -18117, 4147, 273, 13531, 13 );

    m_OriginalTrainTrackData [1][261].Init ( -18130, 4103, 272, 13548, 57 );

    m_OriginalTrainTrackData [1][262].Init ( -18179, 4072, 272, 13570, 48 );

    m_OriginalTrainTrackData [1][263].Init ( -18213, 4070, 272, 13583, 55 );

    m_OriginalTrainTrackData [1][264].Init ( -18373, 4068, 272, 13643, 50 );

    m_OriginalTrainTrackData [1][265].Init ( -18396, 4068, 270, 13652, 115 );

    m_OriginalTrainTrackData [1][266].Init ( -18533, 4068, 251, 13703, 44 );

    m_OriginalTrainTrackData [1][267].Init ( -18692, 4068, 230, 13763, 32 );

    m_OriginalTrainTrackData [1][268].Init ( -18824, 4068, 229, 13812, 49 );

    m_OriginalTrainTrackData [1][269].Init ( -18850, 4068, 228, 13822, 53 );

    m_OriginalTrainTrackData [1][270].Init ( -18871, 4066, 228, 13830, 49 );

    m_OriginalTrainTrackData [1][271].Init ( -18907, 4062, 228, 13843, 56 );

    m_OriginalTrainTrackData [1][272].Init ( -18928, 4053, 228, 13852, 109 );

    m_OriginalTrainTrackData [1][273].Init ( -18946, 4043, 228, 13860, 32 );

    m_OriginalTrainTrackData [1][274].Init ( -18956, 4023, 229, 13868, 32 );

    m_OriginalTrainTrackData [1][275].Init ( -18960, 4000, 229, 13877, 49 );

    m_OriginalTrainTrackData [1][276].Init ( -18943, 3961, 233, 13893, 50 );

    m_OriginalTrainTrackData [1][277].Init ( -18923, 3920, 237, 13910, 52 );

    m_OriginalTrainTrackData [1][278].Init ( -18838, 3783, 237, 13970, 51 );

    m_OriginalTrainTrackData [1][279].Init ( -18743, 3656, 253, 14030, 109 );

    m_OriginalTrainTrackData [1][280].Init ( -18640, 3533, 266, 14090, 32 );

    m_OriginalTrainTrackData [1][281].Init ( -18531, 3416, 272, 14150, 32 );

    m_OriginalTrainTrackData [1][282].Init ( -18417, 3303, 272, 14210, 49 );

    m_OriginalTrainTrackData [1][283].Init ( -18304, 3190, 272, 14270, 50 );

    m_OriginalTrainTrackData [1][284].Init ( -18191, 3077, 267, 14330, 52 );

    m_OriginalTrainTrackData [1][285].Init ( -18093, 2951, 258, 14390, 57 );

    m_OriginalTrainTrackData [1][286].Init ( -18032, 2801, 255, 14450, 115 );

    m_OriginalTrainTrackData [1][287].Init ( -18011, 2642, 273, 14511, 49 );

    m_OriginalTrainTrackData [1][288].Init ( -18012, 2482, 273, 14571, 44 );

    m_OriginalTrainTrackData [1][289].Init ( -18012, 2242, 273, 14661, 32 );

    m_OriginalTrainTrackData [1][290].Init ( -18011, 1922, 273, 14781, 50 );

    m_OriginalTrainTrackData [1][291].Init ( -18011, 1746, 273, 14847, 49 );

    m_OriginalTrainTrackData [1][292].Init ( -18012, 1586, 273, 14907, 48 );

    m_OriginalTrainTrackData [1][293].Init ( -18012, 1426, 273, 14967, 115 );

    m_OriginalTrainTrackData [1][294].Init ( -18012, 1266, 273, 15027, 32 );

    m_OriginalTrainTrackData [1][295].Init ( -18013, 1106, 273, 15087, 50 );

    m_OriginalTrainTrackData [1][296].Init ( -18014, 946, 273, 15147, 56 );

    m_OriginalTrainTrackData [1][297].Init ( -18016, 786, 273, 15207, 44 );

    m_OriginalTrainTrackData [1][298].Init ( -18022, 626, 273, 15267, 32 );

    m_OriginalTrainTrackData [1][299].Init ( -18029, 477, 273, 15323, 32 );

    m_OriginalTrainTrackData [1][0].Init ( -18029, 317, 273, 0, 32 );

    m_OriginalTrainTrackData [1][1].Init ( -18035, 157, 273, 60, 54 );

    m_OriginalTrainTrackData [1][2].Init ( -18039, -2, 273, 117, 53 );

    m_OriginalTrainTrackData [1][3].Init ( -18044, -162, 273, 177, 32 );

    m_OriginalTrainTrackData [1][4].Init ( -18044, -322, 273, 237, 54 );

    m_OriginalTrainTrackData [1][5].Init ( -18044, -482, 273, 297, 52 );

    m_OriginalTrainTrackData [1][6].Init ( -18035, -506, 273, 306, 52 );

    m_OriginalTrainTrackData [1][7].Init ( -18034, -509, 273, 306, 46 );

    m_OriginalTrainTrackData [1][8].Init ( -18021, -530, 273, 317, 46 );

    m_OriginalTrainTrackData [1][9].Init ( -17995, -548, 273, 328, 54 );

    m_OriginalTrainTrackData [1][10].Init ( -17975, -559, 273, 337, 44 );

    m_OriginalTrainTrackData [1][11].Init ( -17950, -561, 273, 347, 52 );

    m_OriginalTrainTrackData [1][12].Init ( -17897, -561, 273, 365, 54 );

    m_OriginalTrainTrackData [1][13].Init ( -17577, -560, 273, 485, 57 );

    m_OriginalTrainTrackData [1][14].Init ( -17399, -560, 273, 554, 46 );

    m_OriginalTrainTrackData [1][15].Init ( -17374, -553, 273, 563, 49 );

    m_OriginalTrainTrackData [1][16].Init ( -17359, -542, 273, 572, 46 );

    m_OriginalTrainTrackData [1][17].Init ( -17345, -527, 273, 578, 83 );

    m_OriginalTrainTrackData [1][18].Init ( -17336, -509, 273, 585, 32 );

    m_OriginalTrainTrackData [1][19].Init ( -17333, -482, 273, 595, 49 );

    m_OriginalTrainTrackData [1][20].Init ( -17333, -317, 273, 658, 57 );

    m_OriginalTrainTrackData [1][21].Init ( -17333, -157, 273, 718, 51 );

    m_OriginalTrainTrackData [1][22].Init ( -17333, 2, 273, 775, 55 );

    m_OriginalTrainTrackData [1][23].Init ( -17332, 162, 273, 835, 46 );

    m_OriginalTrainTrackData [1][24].Init ( -17326, 189, 273, 844, 83 );

    m_OriginalTrainTrackData [1][25].Init ( -17312, 217, 273, 856, 32 );

    m_OriginalTrainTrackData [1][26].Init ( -17290, 237, 273, 867, 49 );

    m_OriginalTrainTrackData [1][27].Init ( -17257, 244, 273, 880, 48 );

    m_OriginalTrainTrackData [1][28].Init ( -17097, 243, 273, 940, 57 );

    m_OriginalTrainTrackData [1][29].Init ( -16937, 243, 273, 1000, 46 );

    m_OriginalTrainTrackData [1][30].Init ( -16777, 243, 273, 1060, 54 );

    m_OriginalTrainTrackData [1][31].Init ( -16617, 243, 273, 1120, 44 );

    m_OriginalTrainTrackData [1][32].Init ( -16457, 243, 273, 1180, 49 );

    m_OriginalTrainTrackData [1][33].Init ( -16297, 243, 273, 1240, 52 );

    m_OriginalTrainTrackData [1][34].Init ( -16137, 230, 254, 1300, 46 );

    m_OriginalTrainTrackData [1][35].Init ( -16109, 228, 254, 1312, 51 );

    m_OriginalTrainTrackData [1][36].Init ( -16086, 236, 254, 1321, 57 );

    m_OriginalTrainTrackData [1][37].Init ( -16070, 252, 254, 1330, 52 );

    m_OriginalTrainTrackData [1][38].Init ( -16060, 272, 254, 1339, 100 );

    m_OriginalTrainTrackData [1][39].Init ( -16054, 288, 254, 1346, 32 );

    m_OriginalTrainTrackData [1][40].Init ( -16053, 307, 254, 1352, 32 );

    m_OriginalTrainTrackData [1][41].Init ( -16052, 467, 231, 1412, 48 );

    m_OriginalTrainTrackData [1][42].Init ( -16052, 627, 212, 1472, 45 );

    m_OriginalTrainTrackData [1][43].Init ( -16052, 787, 212, 1532, 52 );

    m_OriginalTrainTrackData [1][44].Init ( -16052, 947, 212, 1592, 55 );

    m_OriginalTrainTrackData [1][45].Init ( -16052, 1107, 212, 1652, 104 );

    m_OriginalTrainTrackData [1][46].Init ( -16052, 1267, 212, 1712, 49 );

    m_OriginalTrainTrackData [1][47].Init ( -16052, 1427, 212, 1772, 56 );

    m_OriginalTrainTrackData [1][48].Init ( -16052, 1747, 212, 1892, 44 );

    m_OriginalTrainTrackData [1][49].Init ( -16052, 1907, 223, 1952, 32 );

    m_OriginalTrainTrackData [1][50].Init ( -16039, 2067, 237, 2012, 32 );

    m_OriginalTrainTrackData [1][51].Init ( -16033, 2227, 258, 2072, 49 );

    m_OriginalTrainTrackData [1][52].Init ( -16031, 2379, 269, 2130, 114 );

    m_OriginalTrainTrackData [1][53].Init ( -16029, 2482, 272, 2169, 32 );

    m_OriginalTrainTrackData [1][54].Init ( -16029, 2667, 272, 2238, 53 );

    m_OriginalTrainTrackData [1][55].Init ( -16029, 2867, 272, 2313, 44 );

    m_OriginalTrainTrackData [1][56].Init ( -16029, 3187, 272, 2433, 32 );

    m_OriginalTrainTrackData [1][57].Init ( -16029, 3347, 272, 2493, 49 );

    m_OriginalTrainTrackData [1][58].Init ( -16029, 3507, 272, 2553, 49 );

    m_OriginalTrainTrackData [1][59].Init ( -16029, 3667, 272, 2613, 79 );

    m_OriginalTrainTrackData [1][60].Init ( -16029, 3827, 272, 2673, 44 );

    m_OriginalTrainTrackData [1][61].Init ( -16030, 3953, 272, 2721, 45 );

    m_OriginalTrainTrackData [1][62].Init ( -16030, 4113, 272, 2781, 49 );

    m_OriginalTrainTrackData [1][63].Init ( -16030, 4249, 272, 2832, 53 );

    m_OriginalTrainTrackData [1][64].Init ( -16030, 4267, 272, 2838, 50 );

    m_OriginalTrainTrackData [1][65].Init ( -16010, 4427, 272, 2898, 53 );

    m_OriginalTrainTrackData [1][66].Init ( -16009, 4450, 272, 2907, 116 );

    m_OriginalTrainTrackData [1][67].Init ( -16009, 4627, 272, 2973, 49 );

    m_OriginalTrainTrackData [1][68].Init ( -16009, 4746, 272, 3018, 44 );

    m_OriginalTrainTrackData [1][69].Init ( -16008, 4762, 272, 3024, 44 );

    m_OriginalTrainTrackData [1][70].Init ( -16006, 4778, 272, 3031, 32 );

    m_OriginalTrainTrackData [1][71].Init ( -15993, 4799, 272, 3037, 45 );

    m_OriginalTrainTrackData [1][72].Init ( -15969, 4815, 272, 3048, 49 );

    m_OriginalTrainTrackData [1][73].Init ( -15951, 4822, 272, 3058, 111 );

    m_OriginalTrainTrackData [1][74].Init ( -15932, 4826, 272, 3064, 32 );

    m_OriginalTrainTrackData [1][75].Init ( -14464, 4826, 272, 3613, 56 );

    m_OriginalTrainTrackData [1][76].Init ( -14442, 4826, 271, 3622, 50 );

    m_OriginalTrainTrackData [1][77].Init ( -14423, 4826, 270, 3631, 44 );

    m_OriginalTrainTrackData [1][78].Init ( -14133, 4826, 216, 3739, 44 );

    m_OriginalTrainTrackData [1][79].Init ( -14000, 4826, 192, 3787, 32 );

    m_OriginalTrainTrackData [1][80].Init ( -13984, 4826, 190, 3793, 71 );

    m_OriginalTrainTrackData [1][81].Init ( -13969, 4827, 189, 3799, 48 );

    m_OriginalTrainTrackData [1][82].Init ( -13863, 4850, 189, 3842, 46 );

    m_OriginalTrainTrackData [1][83].Init ( -13776, 4905, 189, 3879, 46 );

    m_OriginalTrainTrackData [1][84].Init ( -13715, 4984, 189, 3917, 53 );

    m_OriginalTrainTrackData [1][85].Init ( -13692, 5087, 189, 3954, 55 );

    m_OriginalTrainTrackData [1][86].Init ( -13693, 5750, 189, 4203, 49 );

    m_OriginalTrainTrackData [1][87].Init ( -13688, 5781, 189, 4215, 13 );

    m_OriginalTrainTrackData [1][88].Init ( -13676, 5803, 189, 4226, 32 );

    m_OriginalTrainTrackData [1][89].Init ( -13653, 5819, 189, 4237, 55 );

    m_OriginalTrainTrackData [1][90].Init ( -13613, 5831, 189, 4252, 51 );

    m_OriginalTrainTrackData [1][91].Init ( -13590, 5830, 187, 4261, 54 );

    m_OriginalTrainTrackData [1][92].Init ( -13166, 5830, 111, 4420, 44 );

    m_OriginalTrainTrackData [1][93].Init ( -13142, 5830, 108, 4429, 44 );

    m_OriginalTrainTrackData [1][94].Init ( -13120, 5830, 107, 4435, 10 );

    m_OriginalTrainTrackData [1][95].Init ( -12989, 5830, 107, 4486, 53 );

    m_OriginalTrainTrackData [1][96].Init ( -12967, 5830, 108, 4495, 52 );

    m_OriginalTrainTrackData [1][97].Init ( -12945, 5830, 105, 4501, 50 );

    m_OriginalTrainTrackData [1][98].Init ( -12653, 5830, 52, 4612, 44 );

    m_OriginalTrainTrackData [1][99].Init ( -12571, 5830, 48, 4642, 32 );

    m_OriginalTrainTrackData [1][100].Init ( -12443, 5830, 48, 4690, 50 );

    m_OriginalTrainTrackData [1][101].Init ( -12411, 5836, 48, 4703, 97 );

    m_OriginalTrainTrackData [1][102].Init ( -12386, 5852, 48, 4713, 44 );

    m_OriginalTrainTrackData [1][103].Init ( -12368, 5879, 48, 4724, 50 );

    m_OriginalTrainTrackData [1][104].Init ( -12352, 5950, 48, 4752, 52 );

    m_OriginalTrainTrackData [1][105].Init ( -12280, 6669, 48, 5023, 54 );

    m_OriginalTrainTrackData [1][106].Init ( -12293, 6717, 48, 5042, 44 );

    m_OriginalTrainTrackData [1][107].Init ( -12314, 6750, 48, 5057, 32 );

    m_OriginalTrainTrackData [1][108].Init ( -12323, 6757, 48, 5061, 117 );

    m_OriginalTrainTrackData [1][109].Init ( -12346, 6774, 48, 5072, 48 );

    m_OriginalTrainTrackData [1][110].Init ( -12385, 6786, 48, 5088, 46 );

    m_OriginalTrainTrackData [1][111].Init ( -12413, 6792, 48, 5097, 48 );

    m_OriginalTrainTrackData [1][112].Init ( -12559, 6792, 48, 5151, 56 );

    m_OriginalTrainTrackData [1][113].Init ( -12719, 6792, 52, 5211, 44 );

    m_OriginalTrainTrackData [1][114].Init ( -12879, 6791, 52, 5271, 32 );

    m_OriginalTrainTrackData [1][115].Init ( -13017, 6791, 52, 5325, 118 );

    m_OriginalTrainTrackData [1][116].Init ( -13042, 6791, 54, 5334, 54 );

    m_OriginalTrainTrackData [1][117].Init ( -13073, 6791, 64, 5346, 53 );

    m_OriginalTrainTrackData [1][118].Init ( -13351, 6791, 179, 5448, 44 );

    m_OriginalTrainTrackData [1][119].Init ( -13367, 6791, 185, 5454, 32 );

    m_OriginalTrainTrackData [1][120].Init ( -13388, 6791, 190, 5463, 50 );

    m_OriginalTrainTrackData [1][121].Init ( -13408, 6791, 190, 5472, 46 );

    m_OriginalTrainTrackData [1][122].Init ( -14439, 6791, 189, 5856, 116 );

    m_OriginalTrainTrackData [1][123].Init ( -14465, 6791, 191, 5868, 49 );

    m_OriginalTrainTrackData [1][124].Init ( -14492, 6791, 197, 5877, 56 );

    m_OriginalTrainTrackData [1][125].Init ( -14763, 6791, 266, 5979, 44 );

    m_OriginalTrainTrackData [1][126].Init ( -14778, 6791, 270, 5985, 44 );

    m_OriginalTrainTrackData [1][127].Init ( -14791, 6791, 272, 5988, 44 );

    m_OriginalTrainTrackData [1][128].Init ( -14807, 6791, 272, 5994, 32 );

    m_OriginalTrainTrackData [1][129].Init ( -15284, 6791, 272, 6174, 118 );

    m_OriginalTrainTrackData [1][130].Init ( -15303, 6791, 272, 6180, 53 );

    m_OriginalTrainTrackData [1][131].Init ( -15322, 6791, 274, 6189, 55 );

    m_OriginalTrainTrackData [1][132].Init ( -15740, 6791, 350, 6345, 53 );

    m_OriginalTrainTrackData [1][133].Init ( -15752, 6791, 352, 6351, 55 );

    m_OriginalTrainTrackData [1][134].Init ( -15767, 6791, 354, 6354, 50 );

    m_OriginalTrainTrackData [1][135].Init ( -15783, 6791, 354, 6360, 52 );

    m_OriginalTrainTrackData [1][136].Init ( -15933, 6791, 354, 6417, 13 );

    m_OriginalTrainTrackData [1][137].Init ( -15962, 6803, 354, 6431, 32 );

    m_OriginalTrainTrackData [1][138].Init ( -15991, 6827, 354, 6443, 52 );

    m_OriginalTrainTrackData [1][139].Init ( -16005, 6858, 354, 6457, 56 );

    m_OriginalTrainTrackData [1][140].Init ( -16013, 6899, 354, 6472, 49 );

    m_OriginalTrainTrackData [1][141].Init ( -16013, 7247, 354, 6601, 56 );

    m_OriginalTrainTrackData [1][142].Init ( -16007, 7290, 354, 6619, 52 );

    m_OriginalTrainTrackData [1][143].Init ( -15991, 7328, 354, 6636, 48 );

    m_OriginalTrainTrackData [1][144].Init ( -15964, 7356, 354, 6648, 97 );

    m_OriginalTrainTrackData [1][145].Init ( -15933, 7368, 354, 6662, 45 );

    m_OriginalTrainTrackData [1][146].Init ( -15783, 7368, 354, 6719, 52 );

    m_OriginalTrainTrackData [1][147].Init ( -15758, 7368, 353, 6728, 48 );

    m_OriginalTrainTrackData [1][148].Init ( -15739, 7368, 350, 6734, 50 );

    m_OriginalTrainTrackData [1][149].Init ( -15326, 7368, 275, 6890, 52 );

    m_OriginalTrainTrackData [1][150].Init ( -15303, 7368, 272, 6899, 55 );

    m_OriginalTrainTrackData [1][151].Init ( -15283, 7368, 272, 6905, 107 );

    m_OriginalTrainTrackData [1][152].Init ( -14842, 7369, 272, 7070, 45 );

    m_OriginalTrainTrackData [1][153].Init ( -14826, 7369, 272, 7076, 45 );

    m_OriginalTrainTrackData [1][154].Init ( -14811, 7369, 270, 7082, 51 );

    m_OriginalTrainTrackData [1][155].Init ( -14791, 7369, 264, 7091, 50 );

    m_OriginalTrainTrackData [1][156].Init ( -14582, 7369, 199, 7169, 50 );

    m_OriginalTrainTrackData [1][157].Init ( -14562, 7369, 194, 7175, 53 );

    m_OriginalTrainTrackData [1][158].Init ( -14541, 7369, 190, 7184, 97 );

    m_OriginalTrainTrackData [1][159].Init ( -14524, 7369, 189, 7190, 49 );

    m_OriginalTrainTrackData [1][160].Init ( -13327, 7369, 189, 7640, 50 );

    m_OriginalTrainTrackData [1][161].Init ( -13305, 7369, 189, 7646, 57 );

    m_OriginalTrainTrackData [1][162].Init ( -13281, 7369, 185, 7655, 44 );

    m_OriginalTrainTrackData [1][163].Init ( -13259, 7369, 178, 7664, 44 );

    m_OriginalTrainTrackData [1][164].Init ( -12973, 7369, 59, 7772, 44 );

    m_OriginalTrainTrackData [1][165].Init ( -12951, 7369, 54, 7781, 10 );

    m_OriginalTrainTrackData [1][166].Init ( -12930, 7369, 52, 7787, 44 );

    m_OriginalTrainTrackData [1][167].Init ( -12628, 7370, 52, 7901, 49 );

    m_OriginalTrainTrackData [1][168].Init ( -12573, 7370, 48, 7922, 50 );

    m_OriginalTrainTrackData [1][169].Init ( -12289, 7370, 48, 8027, 44 );

    m_OriginalTrainTrackData [1][170].Init ( -12272, 7376, 48, 8033, 32 );

    m_OriginalTrainTrackData [1][171].Init ( -12253, 7386, 48, 8043, 49 );

    m_OriginalTrainTrackData [1][172].Init ( -12238, 7405, 48, 8051, 104 );

    m_OriginalTrainTrackData [1][173].Init ( -12230, 7431, 48, 8061, 49 );

    m_OriginalTrainTrackData [1][174].Init ( -12226, 7458, 48, 8073, 44 );

    m_OriginalTrainTrackData [1][175].Init ( -12226, 7550, 48, 8106, 32 );

    m_OriginalTrainTrackData [1][176].Init ( -12247, 7606, 48, 8128, 49 );

    m_OriginalTrainTrackData [1][177].Init ( -12286, 7694, 48, 8164, 49 );

    m_OriginalTrainTrackData [1][178].Init ( -12325, 7769, 48, 8198, 46 );

    m_OriginalTrainTrackData [1][179].Init ( -12441, 7925, 48, 8270, 115 );

    m_OriginalTrainTrackData [1][180].Init ( -12506, 7991, 48, 8304, 49 );

    m_OriginalTrainTrackData [1][181].Init ( -12591, 8097, 48, 8356, 48 );

    m_OriginalTrainTrackData [1][182].Init ( -12648, 8186, 48, 8396, 54 );

    m_OriginalTrainTrackData [1][183].Init ( -12673, 8280, 48, 8434, 54 );

    m_OriginalTrainTrackData [1][184].Init ( -12673, 9240, 48, 8794, 46 );

    m_OriginalTrainTrackData [1][185].Init ( -12689, 9378, 48, 8845, 56 );

    m_OriginalTrainTrackData [1][186].Init ( -12711, 9485, 48, 8884, 32 );

    m_OriginalTrainTrackData [1][187].Init ( -12756, 9587, 48, 8927, 54 );

    m_OriginalTrainTrackData [1][188].Init ( -12811, 9691, 48, 8972, 46 );

    m_OriginalTrainTrackData [1][189].Init ( -12889, 9792, 48, 9021, 56 );

    m_OriginalTrainTrackData [1][190].Init ( -13792, 10698, 48, 9500, 54 );

    m_OriginalTrainTrackData [1][191].Init ( -13906, 10811, 48, 9560, 44 );

    m_OriginalTrainTrackData [1][192].Init ( -14116, 10951, 48, 9653, 10 );

    m_OriginalTrainTrackData [1][193].Init ( -14292, 11024, 48, 9725, 54 );

    m_OriginalTrainTrackData [1][194].Init ( -14480, 11047, 48, 9798, 46 );

    m_OriginalTrainTrackData [1][195].Init ( -14669, 11023, 48, 9867, 46 );

    m_OriginalTrainTrackData [1][196].Init ( -14846, 10950, 48, 9939, 53 );

    m_OriginalTrainTrackData [1][197].Init ( -14997, 10833, 48, 10009, 53 );

    m_OriginalTrainTrackData [1][198].Init ( -15111, 10721, 48, 10069, 53 );

    m_OriginalTrainTrackData [1][199].Init ( -15223, 10607, 48, 10130, 13 );

    m_OriginalTrainTrackData [1][200].Init ( -15325, 10528, 48, 10178, 53 );

    m_OriginalTrainTrackData [1][201].Init ( -15444, 10481, 48, 10226, 53 );

    m_OriginalTrainTrackData [1][202].Init ( -15571, 10463, 48, 10275, 49 );

    m_OriginalTrainTrackData [1][203].Init ( -16051, 10463, 48, 10455, 54 );

    m_OriginalTrainTrackData [1][204].Init ( -16188, 10468, 48, 10506, 51 );

    m_OriginalTrainTrackData [1][205].Init ( -16282, 10472, 49, 10542, 44 );

    m_OriginalTrainTrackData [1][206].Init ( -16302, 10472, 49, 10548, 65 );

    m_OriginalTrainTrackData [1][207].Init ( -16322, 10468, 49, 10558, 44 );

    m_OriginalTrainTrackData [1][208].Init ( -16341, 10456, 49, 10564, 49 );

    m_OriginalTrainTrackData [1][209].Init ( -16359, 10438, 49, 10575, 51 );

    m_OriginalTrainTrackData [1][210].Init ( -16439, 10312, 49, 10629, 57 );

    m_OriginalTrainTrackData [1][211].Init ( -16491, 10243, 53, 10664, 46 );

    m_OriginalTrainTrackData [1][212].Init ( -16581, 10199, 73, 10701, 51 );

    m_OriginalTrainTrackData [1][213].Init ( -16684, 10194, 99, 10740, 44 );

    m_OriginalTrainTrackData [1][214].Init ( -16980, 10194, 153, 10851, 50 );

    m_OriginalTrainTrackData [1][215].Init ( -17265, 10194, 205, 10959, 52 );

    m_OriginalTrainTrackData [1][216].Init ( -17437, 10193, 228, 11022, 54 );

    m_OriginalTrainTrackData [1][217].Init ( -17785, 10194, 290, 11154, 46 );

    m_OriginalTrainTrackData [1][218].Init ( -17900, 10194, 309, 11196, 55 );

    m_OriginalTrainTrackData [1][219].Init ( -18012, 10169, 322, 11239, 57 );

    m_OriginalTrainTrackData [1][220].Init ( -18117, 10099, 333, 11286, 44 );

    m_OriginalTrainTrackData [1][221].Init ( -18173, 9997, 344, 11331, 55 );

    m_OriginalTrainTrackData [1][222].Init ( -18187, 9900, 358, 11367, 44 );

    m_OriginalTrainTrackData [1][223].Init ( -18178, 9763, 383, 11418, 32 );

    m_OriginalTrainTrackData [1][224].Init ( -18135, 9603, 428, 11481, 51 );

    m_OriginalTrainTrackData [1][225].Init ( -18118, 9525, 436, 11512, 48 );

    m_OriginalTrainTrackData [1][226].Init ( -18119, 9320, 438, 11587, 50 );

    m_OriginalTrainTrackData [1][227].Init ( -18119, 9295, 441, 11599, 50 );

    m_OriginalTrainTrackData [1][228].Init ( -18118, 9270, 449, 11608, 48 );

    m_OriginalTrainTrackData [1][229].Init ( -18119, 8868, 617, 11758, 55 );

    m_OriginalTrainTrackData [1][230].Init ( -18119, 8847, 626, 11767, 46 );

    m_OriginalTrainTrackData [1][231].Init ( -18119, 8827, 630, 11773, 56 );

    m_OriginalTrainTrackData [1][232].Init ( -18119, 8807, 630, 11782, 56 );

    m_OriginalTrainTrackData [1][233].Init ( -18119, 8667, 630, 11833, 51 );

    m_OriginalTrainTrackData [1][234].Init ( -18119, 8387, 661, 11938, 52 );

    m_OriginalTrainTrackData [1][235].Init ( -18119, 8227, 661, 11998, 50 );

    m_OriginalTrainTrackData [1][236].Init ( -18119, 8106, 662, 12043, 50 );

    m_OriginalTrainTrackData [1][237].Init ( -18119, 8081, 660, 12052, 44 );

    m_OriginalTrainTrackData [1][238].Init ( -18119, 8061, 653, 12061, 32 );

    m_OriginalTrainTrackData [1][239].Init ( -18119, 7785, 538, 12163, 71 );

    m_OriginalTrainTrackData [1][240].Init ( -18119, 7764, 532, 12172, 44 );

    m_OriginalTrainTrackData [1][241].Init ( -18119, 7746, 528, 12178, 32 );

    m_OriginalTrainTrackData [1][242].Init ( -18119, 7668, 524, 12208, 57 );

    m_OriginalTrainTrackData [1][243].Init ( -18119, 6910, 523, 12493, 50 );

    m_OriginalTrainTrackData [1][244].Init ( -18119, 6896, 523, 12496, 46 );

    m_OriginalTrainTrackData [1][245].Init ( -18119, 6881, 520, 12502, 46 );

    m_OriginalTrainTrackData [1][246].Init ( -18119, 6866, 516, 12508, 50 );

    m_OriginalTrainTrackData [1][247].Init ( -18119, 6854, 512, 12514, 46 );

    m_OriginalTrainTrackData [1][248].Init ( -18119, 6578, 397, 12616, 52 );

    m_OriginalTrainTrackData [1][249].Init ( -18119, 6558, 390, 12625, 50 );

    m_OriginalTrainTrackData [1][250].Init ( -18119, 6543, 387, 12631, 44 );

    m_OriginalTrainTrackData [1][251].Init ( -18119, 6527, 387, 12637, 32 );

    m_OriginalTrainTrackData [1][252].Init ( -18120, 5427, 386, 13048, 71 );

    m_OriginalTrainTrackData [1][253].Init ( -18119, 5139, 386, 13156, 44 );

    m_OriginalTrainTrackData [1][254].Init ( -18119, 5109, 386, 13168, 32 );

    m_OriginalTrainTrackData [1][255].Init ( -18119, 5080, 382, 13177, 57 );

    m_OriginalTrainTrackData [1][256].Init ( -18119, 4650, 275, 13339, 53 );

    m_OriginalTrainTrackData [1][257].Init ( -18119, 4586, 272, 13363, 51 );

    m_OriginalTrainTrackData [1][258].Init ( -18119, 4527, 272, 13387, 55 );

    m_OriginalTrainTrackData [1][259].Init ( -18119, 4427, 272, 13423, 10 );

    m_OriginalTrainTrackData [1][260].Init ( -18117, 4147, 273, 13528, 13 );

    m_OriginalTrainTrackData [1][261].Init ( -18130, 4103, 272, 13547, 57 );

    m_OriginalTrainTrackData [1][262].Init ( -18179, 4072, 272, 13567, 48 );

    m_OriginalTrainTrackData [1][263].Init ( -18213, 4070, 272, 13579, 55 );

    m_OriginalTrainTrackData [1][264].Init ( -18373, 4068, 272, 13639, 50 );

    m_OriginalTrainTrackData [1][265].Init ( -18396, 4068, 270, 13648, 115 );

    m_OriginalTrainTrackData [1][266].Init ( -18533, 4068, 251, 13699, 44 );

    m_OriginalTrainTrackData [1][267].Init ( -18692, 4068, 230, 13759, 32 );

    m_OriginalTrainTrackData [1][268].Init ( -18824, 4068, 229, 13810, 49 );

    m_OriginalTrainTrackData [1][269].Init ( -18850, 4068, 228, 13819, 53 );

    m_OriginalTrainTrackData [1][270].Init ( -18871, 4066, 228, 13825, 49 );

    m_OriginalTrainTrackData [1][271].Init ( -18907, 4062, 228, 13840, 56 );

    m_OriginalTrainTrackData [1][272].Init ( -18928, 4053, 228, 13850, 109 );

    m_OriginalTrainTrackData [1][273].Init ( -18946, 4043, 228, 13857, 32 );

    m_OriginalTrainTrackData [1][274].Init ( -18956, 4023, 229, 13866, 32 );

    m_OriginalTrainTrackData [1][275].Init ( -18960, 4000, 229, 13873, 49 );

    m_OriginalTrainTrackData [1][276].Init ( -18943, 3961, 233, 13890, 50 );

    m_OriginalTrainTrackData [1][277].Init ( -18923, 3920, 237, 13906, 52 );

    m_OriginalTrainTrackData [1][278].Init ( -18838, 3783, 237, 13970, 51 );

    m_OriginalTrainTrackData [1][279].Init ( -18743, 3656, 253, 14027, 109 );

    m_OriginalTrainTrackData [1][280].Init ( -18640, 3533, 266, 14087, 32 );

    m_OriginalTrainTrackData [1][281].Init ( -18531, 3416, 272, 14147, 32 );

    m_OriginalTrainTrackData [1][282].Init ( -18417, 3303, 272, 14208, 49 );

    m_OriginalTrainTrackData [1][283].Init ( -18304, 3190, 272, 14268, 50 );

    m_OriginalTrainTrackData [1][284].Init ( -18191, 3077, 267, 14329, 52 );

    m_OriginalTrainTrackData [1][285].Init ( -18093, 2951, 258, 14389, 57 );

    m_OriginalTrainTrackData [1][286].Init ( -18032, 2801, 255, 14447, 115 );

    m_OriginalTrainTrackData [1][287].Init ( -18011, 2642, 273, 14508, 49 );

    m_OriginalTrainTrackData [1][288].Init ( -18012, 2482, 273, 14568, 44 );

    m_OriginalTrainTrackData [1][289].Init ( -18012, 2242, 273, 14658, 32 );

    m_OriginalTrainTrackData [1][290].Init ( -18011, 1922, 273, 14778, 50 );

    m_OriginalTrainTrackData [1][291].Init ( -18011, 1746, 273, 14844, 49 );

    m_OriginalTrainTrackData [1][292].Init ( -18012, 1586, 273, 14904, 48 );

    m_OriginalTrainTrackData [1][293].Init ( -18012, 1426, 273, 14964, 115 );

    m_OriginalTrainTrackData [1][294].Init ( -18012, 1266, 273, 15024, 32 );

    m_OriginalTrainTrackData [1][295].Init ( -18013, 1106, 273, 15084, 50 );

    m_OriginalTrainTrackData [1][296].Init ( -18014, 946, 273, 15144, 56 );

    m_OriginalTrainTrackData [1][297].Init ( -18016, 786, 273, 15204, 44 );

    m_OriginalTrainTrackData [1][298].Init ( -18022, 626, 273, 15264, 32 );

    m_OriginalTrainTrackData [1][299].Init ( -18029, 477, 273, 15321, 32 );
    m_OriginalTrainTrackData [2][0].Init ( -12312, 1073, 22, 0, 32 );

    m_OriginalTrainTrackData [2][1].Init ( -12409, 974, 22, 51, 48 );

    m_OriginalTrainTrackData [2][2].Init ( -12499, 887, 22, 98, 49 );

    m_OriginalTrainTrackData [2][3].Init ( -12589, 796, 22, 146, 46 );

    m_OriginalTrainTrackData [2][4].Init ( -12681, 703, 22, 195, 55 );

    m_OriginalTrainTrackData [2][5].Init ( -12772, 614, 22, 243, 51 );

    m_OriginalTrainTrackData [2][6].Init ( -12844, 539, 22, 282, 51 );

    m_OriginalTrainTrackData [2][7].Init ( -12894, 493, 22, 308, 44 );

    m_OriginalTrainTrackData [2][8].Init ( -12947, 438, 23, 336, 51 );

    m_OriginalTrainTrackData [2][9].Init ( -12997, 386, 22, 363, 49 );

    m_OriginalTrainTrackData [2][10].Init ( -13086, 297, 22, 411, 52 );

    m_OriginalTrainTrackData [2][11].Init ( -13223, 162, 22, 483, 50 );

    m_OriginalTrainTrackData [2][12].Init ( -13305, 89, 22, 524, 50 );

    m_OriginalTrainTrackData [2][13].Init ( -13397, 7, 22, 570, 46 );

    m_OriginalTrainTrackData [2][14].Init ( -13474, -66, 22, 610, 109 );

    m_OriginalTrainTrackData [2][15].Init ( -13546, -138, 22, 648, 32 );

    m_OriginalTrainTrackData [2][16].Init ( -13629, -203, 22, 688, 45 );

    m_OriginalTrainTrackData [2][17].Init ( -13713, -248, 22, 723, 49 );

    m_OriginalTrainTrackData [2][18].Init ( -13811, -267, 22, 761, 50 );

    m_OriginalTrainTrackData [2][19].Init ( -13923, -267, 22, 803, 49 );

    m_OriginalTrainTrackData [2][20].Init ( -14040, -267, 22, 847, 56 );

    m_OriginalTrainTrackData [2][21].Init ( -14189, -267, 27, 902, 98 );

    m_OriginalTrainTrackData [2][22].Init ( -14506, -267, 51, 1021, 44 );

    m_OriginalTrainTrackData [2][23].Init ( -14605, -267, 64, 1058, 45 );

    m_OriginalTrainTrackData [2][24].Init ( -14734, -268, 83, 1107, 49 );

    m_OriginalTrainTrackData [2][25].Init ( -14935, -268, 113, 1182, 51 );

    m_OriginalTrainTrackData [2][26].Init ( -15040, -268, 131, 1222, 49 );

    m_OriginalTrainTrackData [2][27].Init ( -15128, -268, 145, 1255, 53 );

    m_OriginalTrainTrackData [2][28].Init ( -15152, -267, 149, 1264, 111 );

    m_OriginalTrainTrackData [2][29].Init ( -15177, -265, 154, 1273, 44 );

    m_OriginalTrainTrackData [2][30].Init ( -15216, -258, 161, 1288, 32 );

    m_OriginalTrainTrackData [2][31].Init ( -15256, -251, 167, 1303, 32 );

    m_OriginalTrainTrackData [2][32].Init ( -15293, -237, 173, 1318, 50 );

    m_OriginalTrainTrackData [2][33].Init ( -15329, -218, 177, 1333, 49 );

    m_OriginalTrainTrackData [2][34].Init ( -15358, -194, 181, 1347, 48 );

    m_OriginalTrainTrackData [2][35].Init ( -15377, -161, 184, 1362, 118 );

    m_OriginalTrainTrackData [2][36].Init ( -15391, -128, 187, 1375, 32 );

    m_OriginalTrainTrackData [2][37].Init ( -15400, -93, 190, 1389, 49 );

    m_OriginalTrainTrackData [2][38].Init ( -15410, -29, 194, 1413, 55 );

    m_OriginalTrainTrackData [2][39].Init ( -15419, 26, 197, 1434, 44 );

    m_OriginalTrainTrackData [2][40].Init ( -15428, 85, 197, 1456, 32 );

    m_OriginalTrainTrackData [2][41].Init ( -15435, 149, 197, 1480, 32 );

    m_OriginalTrainTrackData [2][42].Init ( -15440, 196, 197, 1498, 115 );

    m_OriginalTrainTrackData [2][43].Init ( -15441, 222, 197, 1508, 44 );

    m_OriginalTrainTrackData [2][44].Init ( -15443, 239, 197, 1514, 50 );

    m_OriginalTrainTrackData [2][45].Init ( -15449, 255, 197, 1521, 48 );

    m_OriginalTrainTrackData [2][46].Init ( -15454, 271, 197, 1527, 51 );

    m_OriginalTrainTrackData [2][47].Init ( -15460, 288, 197, 1534, 44 );

    m_OriginalTrainTrackData [2][48].Init ( -15467, 307, 197, 1541, 44 );

    m_OriginalTrainTrackData [2][49].Init ( -15470, 328, 197, 1549, 10 );

    m_OriginalTrainTrackData [2][50].Init ( -15472, 341, 197, 1554, 50 );

    m_OriginalTrainTrackData [2][51].Init ( -15472, 361, 197, 1562, 50 );

    m_OriginalTrainTrackData [2][52].Init ( -15473, 379, 197, 1568, 52 );

    m_OriginalTrainTrackData [2][53].Init ( -15472, 402, 197, 1577, 55 );

    m_OriginalTrainTrackData [2][54].Init ( -15472, 415, 197, 1582, 56 );

    m_OriginalTrainTrackData [2][55].Init ( -15473, 434, 197, 1589, 49 );

    m_OriginalTrainTrackData [2][56].Init ( -15473, 462, 197, 1600, 118 );

    m_OriginalTrainTrackData [2][57].Init ( -15473, 500, 197, 1614, 44 );

    m_OriginalTrainTrackData [2][58].Init ( -15473, 536, 197, 1627, 49 );

    m_OriginalTrainTrackData [2][59].Init ( -15472, 583, 197, 1645, 52 );

    m_OriginalTrainTrackData [2][60].Init ( -15473, 625, 197, 1661, 44 );

    m_OriginalTrainTrackData [2][61].Init ( -15472, 713, 197, 1694, 32 );

    m_OriginalTrainTrackData [2][62].Init ( -15472, 835, 197, 1739, 53 );

    m_OriginalTrainTrackData [2][63].Init ( -15471, 989, 197, 1797, 103 );

    m_OriginalTrainTrackData [2][64].Init ( -15468, 1119, 197, 1846, 49 );

    m_OriginalTrainTrackData [2][65].Init ( -15465, 1235, 197, 1889, 44 );

    m_OriginalTrainTrackData [2][66].Init ( -15457, 1391, 197, 1948, 44 );

    m_OriginalTrainTrackData [2][0].Init ( -12312, 1073, 22, 0, 32 );

    m_OriginalTrainTrackData [2][1].Init ( -12409, 974, 22, 51, 48 );

    m_OriginalTrainTrackData [2][2].Init ( -12499, 887, 22, 98, 49 );

    m_OriginalTrainTrackData [2][3].Init ( -12589, 796, 22, 146, 46 );

    m_OriginalTrainTrackData [2][4].Init ( -12681, 703, 22, 195, 55 );

    m_OriginalTrainTrackData [2][5].Init ( -12772, 614, 22, 243, 51 );

    m_OriginalTrainTrackData [2][6].Init ( -12844, 539, 22, 282, 51 );

    m_OriginalTrainTrackData [2][7].Init ( -12894, 493, 22, 308, 44 );

    m_OriginalTrainTrackData [2][8].Init ( -12947, 438, 23, 336, 51 );

    m_OriginalTrainTrackData [2][9].Init ( -12997, 386, 22, 363, 49 );

    m_OriginalTrainTrackData [2][10].Init ( -13086, 297, 22, 411, 52 );

    m_OriginalTrainTrackData [2][11].Init ( -13223, 162, 22, 483, 50 );

    m_OriginalTrainTrackData [2][12].Init ( -13305, 89, 22, 524, 50 );

    m_OriginalTrainTrackData [2][13].Init ( -13397, 7, 22, 570, 46 );

    m_OriginalTrainTrackData [2][14].Init ( -13474, -66, 22, 610, 109 );

    m_OriginalTrainTrackData [2][15].Init ( -13546, -138, 22, 648, 32 );

    m_OriginalTrainTrackData [2][16].Init ( -13629, -203, 22, 688, 45 );

    m_OriginalTrainTrackData [2][17].Init ( -13713, -248, 22, 723, 49 );

    m_OriginalTrainTrackData [2][18].Init ( -13811, -267, 22, 761, 50 );

    m_OriginalTrainTrackData [2][19].Init ( -13923, -267, 22, 803, 49 );

    m_OriginalTrainTrackData [2][20].Init ( -14040, -267, 22, 847, 56 );

    m_OriginalTrainTrackData [2][21].Init ( -14189, -267, 27, 902, 98 );

    m_OriginalTrainTrackData [2][22].Init ( -14506, -267, 51, 1021, 44 );

    m_OriginalTrainTrackData [2][23].Init ( -14605, -267, 64, 1058, 45 );

    m_OriginalTrainTrackData [2][24].Init ( -14734, -268, 83, 1107, 49 );

    m_OriginalTrainTrackData [2][25].Init ( -14935, -268, 113, 1182, 51 );

    m_OriginalTrainTrackData [2][26].Init ( -15040, -268, 131, 1222, 49 );

    m_OriginalTrainTrackData [2][27].Init ( -15128, -268, 145, 1255, 53 );

    m_OriginalTrainTrackData [2][28].Init ( -15152, -267, 149, 1264, 111 );

    m_OriginalTrainTrackData [2][29].Init ( -15177, -265, 154, 1273, 44 );

    m_OriginalTrainTrackData [2][30].Init ( -15216, -258, 161, 1288, 32 );

    m_OriginalTrainTrackData [2][31].Init ( -15256, -251, 167, 1303, 32 );

    m_OriginalTrainTrackData [2][32].Init ( -15293, -237, 173, 1318, 50 );

    m_OriginalTrainTrackData [2][33].Init ( -15329, -218, 177, 1333, 49 );

    m_OriginalTrainTrackData [2][34].Init ( -15358, -194, 181, 1347, 48 );

    m_OriginalTrainTrackData [2][35].Init ( -15377, -161, 184, 1362, 118 );

    m_OriginalTrainTrackData [2][36].Init ( -15391, -128, 187, 1375, 32 );

    m_OriginalTrainTrackData [2][37].Init ( -15400, -93, 190, 1389, 49 );

    m_OriginalTrainTrackData [2][38].Init ( -15410, -29, 194, 1413, 55 );

    m_OriginalTrainTrackData [2][39].Init ( -15419, 26, 197, 1434, 44 );

    m_OriginalTrainTrackData [2][40].Init ( -15428, 85, 197, 1456, 32 );

    m_OriginalTrainTrackData [2][41].Init ( -15435, 149, 197, 1480, 32 );

    m_OriginalTrainTrackData [2][42].Init ( -15440, 196, 197, 1498, 115 );

    m_OriginalTrainTrackData [2][43].Init ( -15441, 222, 197, 1508, 44 );

    m_OriginalTrainTrackData [2][44].Init ( -15443, 239, 197, 1514, 50 );

    m_OriginalTrainTrackData [2][45].Init ( -15449, 255, 197, 1521, 48 );

    m_OriginalTrainTrackData [2][46].Init ( -15454, 271, 197, 1527, 51 );

    m_OriginalTrainTrackData [2][47].Init ( -15460, 288, 197, 1534, 44 );

    m_OriginalTrainTrackData [2][48].Init ( -15467, 307, 197, 1541, 44 );

    m_OriginalTrainTrackData [2][49].Init ( -15470, 328, 197, 1549, 10 );

    m_OriginalTrainTrackData [2][50].Init ( -15472, 341, 197, 1554, 50 );

    m_OriginalTrainTrackData [2][51].Init ( -15472, 361, 197, 1562, 50 );

    m_OriginalTrainTrackData [2][52].Init ( -15473, 379, 197, 1568, 52 );

    m_OriginalTrainTrackData [2][53].Init ( -15472, 402, 197, 1577, 55 );

    m_OriginalTrainTrackData [2][54].Init ( -15472, 415, 197, 1582, 56 );

    m_OriginalTrainTrackData [2][55].Init ( -15473, 434, 197, 1589, 49 );

    m_OriginalTrainTrackData [2][56].Init ( -15473, 462, 197, 1600, 118 );

    m_OriginalTrainTrackData [2][57].Init ( -15473, 500, 197, 1614, 44 );

    m_OriginalTrainTrackData [2][58].Init ( -15473, 536, 197, 1627, 49 );

    m_OriginalTrainTrackData [2][59].Init ( -15472, 583, 197, 1645, 52 );

    m_OriginalTrainTrackData [2][60].Init ( -15473, 625, 197, 1661, 44 );

    m_OriginalTrainTrackData [2][61].Init ( -15472, 713, 197, 1694, 32 );

    m_OriginalTrainTrackData [2][62].Init ( -15472, 835, 197, 1739, 53 );

    m_OriginalTrainTrackData [2][63].Init ( -15471, 989, 197, 1797, 103 );

    m_OriginalTrainTrackData [2][64].Init ( -15468, 1119, 197, 1846, 49 );

    m_OriginalTrainTrackData [2][65].Init ( -15465, 1235, 197, 1889, 44 );

    m_OriginalTrainTrackData [2][66].Init ( -15457, 1391, 197, 1948, 44 );

    m_OriginalTrainTrackData [2][0].Init ( -12312, 1073, 22, 0, 32 );

    m_OriginalTrainTrackData [2][1].Init ( -12409, 974, 22, 53, 48 );

    m_OriginalTrainTrackData [2][2].Init ( -12499, 887, 22, 99, 49 );

    m_OriginalTrainTrackData [2][3].Init ( -12589, 796, 22, 146, 46 );

    m_OriginalTrainTrackData [2][4].Init ( -12681, 703, 22, 197, 55 );

    m_OriginalTrainTrackData [2][5].Init ( -12772, 614, 22, 243, 51 );

    m_OriginalTrainTrackData [2][6].Init ( -12844, 539, 22, 282, 51 );

    m_OriginalTrainTrackData [2][7].Init ( -12894, 493, 22, 307, 44 );

    m_OriginalTrainTrackData [2][8].Init ( -12947, 438, 23, 337, 51 );

    m_OriginalTrainTrackData [2][9].Init ( -12997, 386, 22, 362, 49 );

    m_OriginalTrainTrackData [2][10].Init ( -13086, 297, 22, 409, 52 );

    m_OriginalTrainTrackData [2][11].Init ( -13223, 162, 22, 481, 50 );

    m_OriginalTrainTrackData [2][12].Init ( -13305, 89, 22, 524, 50 );

    m_OriginalTrainTrackData [2][13].Init ( -13397, 7, 22, 570, 46 );

    m_OriginalTrainTrackData [2][14].Init ( -13474, -66, 22, 609, 109 );

    m_OriginalTrainTrackData [2][15].Init ( -13546, -138, 22, 647, 32 );

    m_OriginalTrainTrackData [2][16].Init ( -13629, -203, 22, 685, 45 );

    m_OriginalTrainTrackData [2][17].Init ( -13713, -248, 22, 723, 49 );

    m_OriginalTrainTrackData [2][18].Init ( -13811, -267, 22, 759, 50 );

    m_OriginalTrainTrackData [2][19].Init ( -13923, -267, 22, 801, 49 );

    m_OriginalTrainTrackData [2][20].Init ( -14040, -267, 22, 846, 56 );

    m_OriginalTrainTrackData [2][21].Init ( -14189, -267, 27, 900, 98 );

    m_OriginalTrainTrackData [2][22].Init ( -14506, -267, 51, 1020, 44 );

    m_OriginalTrainTrackData [2][23].Init ( -14605, -267, 64, 1056, 45 );

    m_OriginalTrainTrackData [2][24].Init ( -14734, -268, 83, 1104, 49 );

    m_OriginalTrainTrackData [2][25].Init ( -14935, -268, 113, 1179, 51 );

    m_OriginalTrainTrackData [2][26].Init ( -15040, -268, 131, 1221, 49 );

    m_OriginalTrainTrackData [2][27].Init ( -15128, -268, 145, 1254, 53 );

    m_OriginalTrainTrackData [2][28].Init ( -15152, -267, 149, 1263, 111 );

    m_OriginalTrainTrackData [2][29].Init ( -15177, -265, 154, 1272, 44 );

    m_OriginalTrainTrackData [2][30].Init ( -15216, -258, 161, 1288, 32 );

    m_OriginalTrainTrackData [2][31].Init ( -15256, -251, 167, 1303, 32 );

    m_OriginalTrainTrackData [2][32].Init ( -15293, -237, 173, 1317, 50 );

    m_OriginalTrainTrackData [2][33].Init ( -15329, -218, 177, 1333, 49 );

    m_OriginalTrainTrackData [2][34].Init ( -15358, -194, 181, 1345, 48 );

    m_OriginalTrainTrackData [2][35].Init ( -15377, -161, 184, 1360, 118 );

    m_OriginalTrainTrackData [2][36].Init ( -15391, -128, 187, 1373, 32 );

    m_OriginalTrainTrackData [2][37].Init ( -15400, -93, 190, 1389, 49 );

    m_OriginalTrainTrackData [2][38].Init ( -15410, -29, 194, 1413, 55 );

    m_OriginalTrainTrackData [2][39].Init ( -15419, 26, 197, 1431, 44 );

    m_OriginalTrainTrackData [2][40].Init ( -15428, 85, 197, 1453, 32 );

    m_OriginalTrainTrackData [2][41].Init ( -15435, 149, 197, 1477, 32 );

    m_OriginalTrainTrackData [2][42].Init ( -15440, 196, 197, 1495, 115 );

    m_OriginalTrainTrackData [2][43].Init ( -15441, 222, 197, 1504, 44 );

    m_OriginalTrainTrackData [2][44].Init ( -15443, 239, 197, 1510, 50 );

    m_OriginalTrainTrackData [2][45].Init ( -15449, 255, 197, 1517, 48 );

    m_OriginalTrainTrackData [2][46].Init ( -15454, 271, 197, 1523, 51 );

    m_OriginalTrainTrackData [2][47].Init ( -15460, 288, 197, 1532, 44 );

    m_OriginalTrainTrackData [2][48].Init ( -15467, 307, 197, 1539, 44 );

    m_OriginalTrainTrackData [2][49].Init ( -15470, 328, 197, 1548, 10 );

    m_OriginalTrainTrackData [2][50].Init ( -15472, 341, 197, 1552, 50 );

    m_OriginalTrainTrackData [2][51].Init ( -15472, 361, 197, 1561, 50 );

    m_OriginalTrainTrackData [2][52].Init ( -15473, 379, 197, 1567, 52 );

    m_OriginalTrainTrackData [2][53].Init ( -15472, 402, 197, 1576, 55 );

    m_OriginalTrainTrackData [2][54].Init ( -15472, 415, 197, 1579, 56 );

    m_OriginalTrainTrackData [2][55].Init ( -15473, 434, 197, 1588, 49 );

    m_OriginalTrainTrackData [2][56].Init ( -15473, 462, 197, 1597, 118 );

    m_OriginalTrainTrackData [2][57].Init ( -15473, 500, 197, 1612, 44 );

    m_OriginalTrainTrackData [2][58].Init ( -15473, 536, 197, 1627, 49 );

    m_OriginalTrainTrackData [2][59].Init ( -15472, 583, 197, 1642, 52 );

    m_OriginalTrainTrackData [2][60].Init ( -15473, 625, 197, 1660, 44 );

    m_OriginalTrainTrackData [2][61].Init ( -15472, 713, 197, 1693, 32 );

    m_OriginalTrainTrackData [2][62].Init ( -15472, 835, 197, 1738, 53 );

    m_OriginalTrainTrackData [2][63].Init ( -15471, 989, 197, 1795, 103 );

    m_OriginalTrainTrackData [2][64].Init ( -15468, 1119, 197, 1843, 49 );

    m_OriginalTrainTrackData [2][65].Init ( -15465, 1235, 197, 1888, 44 );

    m_OriginalTrainTrackData [2][66].Init ( -15457, 1391, 197, 1945, 44 );
    m_OriginalTrainTrackData [3][0].Init ( 22324, -1657, 200, 0, 107 );

    m_OriginalTrainTrackData [3][1].Init ( 22312, -1681, 198, 10, 0 );

    m_OriginalTrainTrackData [3][2].Init ( 22246, -1800, 185, 61, 147 );

    m_OriginalTrainTrackData [3][3].Init ( 22183, -1919, 170, 111, 104 );

    m_OriginalTrainTrackData [3][4].Init ( 22131, -2020, 159, 154, 104 );

    m_OriginalTrainTrackData [3][5].Init ( 22099, -2077, 152, 178, 0 );

    m_OriginalTrainTrackData [3][6].Init ( 22070, -2125, 146, 199, 109 );

    m_OriginalTrainTrackData [3][7].Init ( 22025, -2178, 141, 225, 161 );

    m_OriginalTrainTrackData [3][8].Init ( 21973, -2223, 136, 251, 163 );

    m_OriginalTrainTrackData [3][9].Init ( 21930, -2246, 133, 269, 0 );

    m_OriginalTrainTrackData [3][10].Init ( 21847, -2275, 128, 302, 169 );

    m_OriginalTrainTrackData [3][11].Init ( 21774, -2300, 124, 331, 162 );

    m_OriginalTrainTrackData [3][12].Init ( 21672, -2327, 120, 371, 165 );

    m_OriginalTrainTrackData [3][13].Init ( 21568, -2353, 115, 411, 0 );

    m_OriginalTrainTrackData [3][14].Init ( 21476, -2370, 111, 446, 156 );

    m_OriginalTrainTrackData [3][15].Init ( 21382, -2381, 107, 482, 118 );

    m_OriginalTrainTrackData [3][16].Init ( 21290, -2391, 104, 516, 118 );

    m_OriginalTrainTrackData [3][17].Init ( 21192, -2395, 102, 553, 0 );

    m_OriginalTrainTrackData [3][18].Init ( 21094, -2392, 101, 590, 120 );

    m_OriginalTrainTrackData [3][19].Init ( 20998, -2389, 100, 626, 115 );

    m_OriginalTrainTrackData [3][20].Init ( 20897, -2381, 101, 664, 161 );

    m_OriginalTrainTrackData [3][21].Init ( 20808, -2373, 103, 697, 0 );

    m_OriginalTrainTrackData [3][22].Init ( 20736, -2365, 105, 725, 168 );

    m_OriginalTrainTrackData [3][23].Init ( 20665, -2356, 107, 751, 177 );

    m_OriginalTrainTrackData [3][24].Init ( 20596, -2344, 110, 778, 174 );

    m_OriginalTrainTrackData [3][25].Init ( 20528, -2332, 112, 804, 0 );

    m_OriginalTrainTrackData [3][26].Init ( 20463, -2322, 114, 828, 177 );

    m_OriginalTrainTrackData [3][27].Init ( 20396, -2311, 117, 854, 156 );

    m_OriginalTrainTrackData [3][28].Init ( 20326, -2301, 119, 880, 156 );

    m_OriginalTrainTrackData [3][29].Init ( 20259, -2292, 121, 906, 0 );

    m_OriginalTrainTrackData [3][30].Init ( 20198, -2284, 123, 929, 32 );

    m_OriginalTrainTrackData [3][31].Init ( 20085, -2271, 126, 971, 32 );

    m_OriginalTrainTrackData [3][32].Init ( 19910, -2250, 131, 1037, 25 );

    m_OriginalTrainTrackData [3][33].Init ( 19738, -2231, 135, 1102, 0 );

    m_OriginalTrainTrackData [3][34].Init ( 19620, -2223, 137, 1147, 38 );

    m_OriginalTrainTrackData [3][35].Init ( 19490, -2214, 139, 1195, 20 );

    m_OriginalTrainTrackData [3][36].Init ( 19378, -2207, 140, 1238, 39 );

    m_OriginalTrainTrackData [3][37].Init ( 19255, -2207, 140, 1284, 0 );

    m_OriginalTrainTrackData [3][38].Init ( 19153, -2215, 146, 1322, 39 );

    m_OriginalTrainTrackData [3][39].Init ( 19040, -2237, 151, 1365, 43 );

    m_OriginalTrainTrackData [3][40].Init ( 19001, -2251, 157, 1381, 43 );

    m_OriginalTrainTrackData [3][41].Init ( 18963, -2265, 162, 1396, 0 );

    m_OriginalTrainTrackData [3][42].Init ( 18940, -2274, 165, 1405, 43 );

    m_OriginalTrainTrackData [3][43].Init ( 18913, -2286, 169, 1416, 43 );

    m_OriginalTrainTrackData [3][44].Init ( 18839, -2317, 179, 1446, 32 );

    m_OriginalTrainTrackData [3][45].Init ( 18801, -2338, 187, 1463, 0 );

    m_OriginalTrainTrackData [3][46].Init ( 18746, -2368, 197, 1486, 32 );

    m_OriginalTrainTrackData [3][47].Init ( 18651, -2425, 216, 1528, 37 );

    m_OriginalTrainTrackData [3][48].Init ( 18561, -2487, 234, 1569, 31 );

    m_OriginalTrainTrackData [3][49].Init ( 18465, -2553, 256, 1612, 0 );

    m_OriginalTrainTrackData [3][50].Init ( 18376, -2612, 272, 1652, 49 );

    m_OriginalTrainTrackData [3][51].Init ( 18277, -2675, 290, 1696, 49 );

    m_OriginalTrainTrackData [3][52].Init ( 18189, -2727, 300, 1735, 44 );

    m_OriginalTrainTrackData [3][53].Init ( 18085, -2777, 312, 1778, 0 );

    m_OriginalTrainTrackData [3][54].Init ( 17995, -2814, 320, 1815, 50 );

    m_OriginalTrainTrackData [3][55].Init ( 17892, -2846, 330, 1855, 52 );

    m_OriginalTrainTrackData [3][56].Init ( 17809, -2865, 341, 1887, 53 );

    m_OriginalTrainTrackData [3][57].Init ( 17779, -2870, 345, 1898, 0 );

    m_OriginalTrainTrackData [3][58].Init ( 17714, -2883, 353, 1923, 52 );

    m_OriginalTrainTrackData [3][59].Init ( 17664, -2885, 361, 1942, 52 );

    m_OriginalTrainTrackData [3][60].Init ( 17580, -2893, 374, 1974, 30 );

    m_OriginalTrainTrackData [3][61].Init ( 17531, -2894, 382, 1992, 0 );

    m_OriginalTrainTrackData [3][62].Init ( 17453, -2894, 395, 2021, 62 );

    m_OriginalTrainTrackData [3][63].Init ( 17388, -2893, 406, 2046, 62 );

    m_OriginalTrainTrackData [3][64].Init ( 17321, -2891, 416, 2071, 63 );

    m_OriginalTrainTrackData [3][65].Init ( 17251, -2887, 426, 2097, 0 );

    m_OriginalTrainTrackData [3][66].Init ( 17195, -2885, 434, 2118, 28 );

    m_OriginalTrainTrackData [3][67].Init ( 17134, -2884, 440, 2141, 55 );

    m_OriginalTrainTrackData [3][68].Init ( 17060, -2883, 448, 2169, 56 );

    m_OriginalTrainTrackData [3][69].Init ( 16996, -2885, 454, 2193, 0 );

    m_OriginalTrainTrackData [3][70].Init ( 16911, -2889, 462, 2225, 20 );

    m_OriginalTrainTrackData [3][71].Init ( 16872, -2892, 466, 2239, 20 );

    m_OriginalTrainTrackData [3][72].Init ( 16798, -2900, 474, 2267, 26 );

    m_OriginalTrainTrackData [3][73].Init ( 16766, -2908, 478, 2280, 0 );

    m_OriginalTrainTrackData [3][74].Init ( 16702, -2922, 485, 2304, 192 );

    m_OriginalTrainTrackData [3][75].Init ( 16611, -2947, 496, 2339, 0 );

    m_OriginalTrainTrackData [3][76].Init ( 16517, -2982, 507, 2377, 0 );

    m_OriginalTrainTrackData [3][77].Init ( 16429, -3025, 516, 2414, 153 );

    m_OriginalTrainTrackData [3][78].Init ( 16345, -3076, 527, 2451, 0 );

    m_OriginalTrainTrackData [3][79].Init ( 16269, -3143, 534, 2489, 0 );

    m_OriginalTrainTrackData [3][80].Init ( 16196, -3212, 541, 2526, 46 );

    m_OriginalTrainTrackData [3][81].Init ( 16150, -3283, 543, 2558, 140 );

    m_OriginalTrainTrackData [3][82].Init ( 16129, -3319, 545, 2574, 155 );

    m_OriginalTrainTrackData [3][83].Init ( 16086, -3396, 547, 2607, 40 );

    m_OriginalTrainTrackData [3][84].Init ( 16070, -3441, 548, 2625, 102 );

    m_OriginalTrainTrackData [3][85].Init ( 16045, -3503, 549, 2650, 206 );

    m_OriginalTrainTrackData [3][86].Init ( 16015, -3610, 551, 2691, 46 );

    m_OriginalTrainTrackData [3][87].Init ( 15995, -3702, 555, 2727, 243 );

    m_OriginalTrainTrackData [3][88].Init ( 15981, -3799, 558, 2763, 130 );

    m_OriginalTrainTrackData [3][89].Init ( 15979, -3824, 559, 2773, 46 );

    m_OriginalTrainTrackData [3][90].Init ( 15977, -3912, 563, 2806, 166 );

    m_OriginalTrainTrackData [3][91].Init ( 15983, -3959, 565, 2824, 78 );

    m_OriginalTrainTrackData [3][92].Init ( 15987, -4016, 567, 2845, 46 );

    m_OriginalTrainTrackData [3][93].Init ( 15994, -4045, 568, 2856, 51 );

    m_OriginalTrainTrackData [3][94].Init ( 16016, -4128, 569, 2888, 2 );

    m_OriginalTrainTrackData [3][95].Init ( 16046, -4216, 571, 2923, 46 );

    m_OriginalTrainTrackData [3][96].Init ( 16069, -4258, 570, 2941, 230 );

    m_OriginalTrainTrackData [3][97].Init ( 16103, -4318, 568, 2967, 206 );

    m_OriginalTrainTrackData [3][98].Init ( 16122, -4347, 567, 2980, 46 );

    m_OriginalTrainTrackData [3][99].Init ( 16168, -4418, 566, 3012, 135 );

    m_OriginalTrainTrackData [3][100].Init ( 16201, -4457, 564, 3031, 214 );

    m_OriginalTrainTrackData [3][101].Init ( 16265, -4530, 560, 3067, 193 );

    m_OriginalTrainTrackData [3][102].Init ( 16339, -4603, 555, 3106, 118 );

    m_OriginalTrainTrackData [3][103].Init ( 16371, -4632, 552, 3123, 214 );

    m_OriginalTrainTrackData [3][104].Init ( 16442, -4694, 543, 3158, 231 );

    m_OriginalTrainTrackData [3][105].Init ( 16503, -4748, 536, 3188, 120 );

    m_OriginalTrainTrackData [3][106].Init ( 16576, -4812, 527, 3225, 232 );

    m_OriginalTrainTrackData [3][107].Init ( 16631, -4856, 519, 3251, 27 );

    m_OriginalTrainTrackData [3][108].Init ( 16704, -4920, 507, 3288, 129 );

    m_OriginalTrainTrackData [3][109].Init ( 16787, -4988, 494, 3328, 38 );

    m_OriginalTrainTrackData [3][110].Init ( 16830, -5023, 486, 3349, 28 );

    m_OriginalTrainTrackData [3][111].Init ( 16890, -5071, 476, 3378, 161 );

    m_OriginalTrainTrackData [3][112].Init ( 16935, -5107, 467, 3399, 182 );

    m_OriginalTrainTrackData [3][113].Init ( 16998, -5156, 456, 3429, 36 );

    m_OriginalTrainTrackData [3][114].Init ( 17031, -5184, 450, 3445, 67 );

    m_OriginalTrainTrackData [3][115].Init ( 17109, -5243, 435, 3482, 33 );

    m_OriginalTrainTrackData [3][116].Init ( 17181, -5298, 422, 3516, 38 );

    m_OriginalTrainTrackData [3][117].Init ( 17222, -5330, 414, 3535, 126 );

    m_OriginalTrainTrackData [3][118].Init ( 17292, -5381, 402, 3568, 123 );

    m_OriginalTrainTrackData [3][119].Init ( 17363, -5434, 390, 3601, 36 );

    m_OriginalTrainTrackData [3][120].Init ( 17448, -5499, 375, 3641, 126 );

    m_OriginalTrainTrackData [3][121].Init ( 17478, -5522, 371, 3655, 133 );

    m_OriginalTrainTrackData [3][122].Init ( 17543, -5571, 361, 3686, 28 );

    m_OriginalTrainTrackData [3][123].Init ( 17579, -5597, 356, 3703, 162 );

    m_OriginalTrainTrackData [3][124].Init ( 17644, -5644, 347, 3733, 133 );

    m_OriginalTrainTrackData [3][125].Init ( 17702, -5691, 337, 3761, 3 );

    m_OriginalTrainTrackData [3][126].Init ( 17753, -5732, 330, 3785, 160 );

    m_OriginalTrainTrackData [3][127].Init ( 17797, -5770, 323, 3807, 129 );

    m_OriginalTrainTrackData [3][128].Init ( 17828, -5798, 319, 3823, 14 );

    m_OriginalTrainTrackData [3][129].Init ( 17873, -5838, 313, 3845, 152 );

    m_OriginalTrainTrackData [3][130].Init ( 17906, -5871, 309, 3863, 252 );

    m_OriginalTrainTrackData [3][131].Init ( 17946, -5912, 304, 3884, 14 );

    m_OriginalTrainTrackData [3][132].Init ( 17989, -5968, 297, 3911, 49 );

    m_OriginalTrainTrackData [3][133].Init ( 18034, -6028, 290, 3939, 66 );

    m_OriginalTrainTrackData [3][134].Init ( 18072, -6085, 284, 3965, 192 );

    m_OriginalTrainTrackData [3][135].Init ( 18110, -6147, 277, 3992, 241 );

    m_OriginalTrainTrackData [3][136].Init ( 18137, -6204, 271, 4016, 2 );

    m_OriginalTrainTrackData [3][137].Init ( 18168, -6266, 265, 4042, 132 );

    m_OriginalTrainTrackData [3][138].Init ( 18190, -6330, 260, 4067, 86 );

    m_OriginalTrainTrackData [3][139].Init ( 18214, -6395, 254, 4093, 66 );

    m_OriginalTrainTrackData [3][140].Init ( 18234, -6463, 249, 4119, 132 );

    m_OriginalTrainTrackData [3][141].Init ( 18249, -6528, 244, 4144, 121 );

    m_OriginalTrainTrackData [3][142].Init ( 18262, -6596, 239, 4170, 117 );

    m_OriginalTrainTrackData [3][143].Init ( 18272, -6661, 234, 4195, 128 );

    m_OriginalTrainTrackData [3][144].Init ( 18282, -6722, 230, 4218, 69 );

    m_OriginalTrainTrackData [3][145].Init ( 18289, -6787, 226, 4243, 53 );

    m_OriginalTrainTrackData [3][146].Init ( 18295, -6845, 222, 4265, 128 );

    m_OriginalTrainTrackData [3][147].Init ( 18298, -6884, 220, 4279, 19 );

    m_OriginalTrainTrackData [3][148].Init ( 18300, -6944, 216, 4302, 78 );

    m_OriginalTrainTrackData [3][149].Init ( 18303, -6998, 213, 4322, 101 );

    m_OriginalTrainTrackData [3][150].Init ( 18305, -7049, 210, 4341, 71 );

    m_OriginalTrainTrackData [3][151].Init ( 18308, -7112, 206, 4365, 137 );

    m_OriginalTrainTrackData [3][152].Init ( 18310, -7138, 205, 4375, 128 );

    m_OriginalTrainTrackData [3][153].Init ( 18311, -7180, 202, 4390, 82 );

    m_OriginalTrainTrackData [3][154].Init ( 18311, -7227, 202, 4408, 179 );

    m_OriginalTrainTrackData [3][155].Init ( 18311, -7299, 203, 4435, 128 );

    m_OriginalTrainTrackData [3][156].Init ( 18311, -7342, 203, 4451, 57 );

    m_OriginalTrainTrackData [3][157].Init ( 18311, -7421, 203, 4481, 53 );

    m_OriginalTrainTrackData [3][158].Init ( 18311, -7491, 204, 4507, 133 );

    m_OriginalTrainTrackData [3][159].Init ( 18311, -7568, 204, 4536, 121 );

    m_OriginalTrainTrackData [3][160].Init ( 18311, -7636, 204, 4561, 130 );

    m_OriginalTrainTrackData [3][161].Init ( 18311, -7714, 205, 4591, 8 );

    m_OriginalTrainTrackData [3][162].Init ( 18311, -7768, 205, 4611, 121 );

    m_OriginalTrainTrackData [3][163].Init ( 18311, -7824, 205, 4632, 78 );

    m_OriginalTrainTrackData [3][164].Init ( 18311, -7902, 205, 4661, 187 );

    m_OriginalTrainTrackData [3][165].Init ( 18311, -7937, 205, 4674, 121 );

    m_OriginalTrainTrackData [3][166].Init ( 18311, -7980, 205, 4690, 130 );

    m_OriginalTrainTrackData [3][167].Init ( 18312, -8033, 206, 4710, 70 );

    m_OriginalTrainTrackData [3][168].Init ( 18311, -8121, 206, 4743, 121 );

    m_OriginalTrainTrackData [3][169].Init ( 18311, -8164, 206, 4759, 155 );

    m_OriginalTrainTrackData [3][170].Init ( 18311, -8179, 206, 4765, 187 );

    m_OriginalTrainTrackData [3][171].Init ( 18311, -8222, 206, 4781, 141 );

    m_OriginalTrainTrackData [3][172].Init ( 18311, -8264, 206, 4797, 53 );

    m_OriginalTrainTrackData [3][173].Init ( 18311, -8301, 206, 4811, 16 );

    m_OriginalTrainTrackData [3][174].Init ( 18311, -8340, 206, 4825, 70 );

    m_OriginalTrainTrackData [3][175].Init ( 18311, -8379, 206, 4840, 53 );

    m_OriginalTrainTrackData [3][176].Init ( 18311, -8413, 206, 4853, 128 );

    m_OriginalTrainTrackData [3][177].Init ( 18310, -8461, 206, 4871, 142 );

    m_OriginalTrainTrackData [3][178].Init ( 18310, -8516, 206, 4891, 130 );

    m_OriginalTrainTrackData [3][179].Init ( 18310, -8580, 207, 4915, 181 );

    m_OriginalTrainTrackData [3][180].Init ( 18310, -8639, 207, 4938, 142 );

    m_OriginalTrainTrackData [3][181].Init ( 18311, -8732, 207, 4972, 66 );

    m_OriginalTrainTrackData [3][182].Init ( 18310, -8753, 207, 4980, 142 );

    m_OriginalTrainTrackData [3][183].Init ( 18311, -8809, 205, 5001, 142 );

    m_OriginalTrainTrackData [3][184].Init ( 18311, -8845, 207, 5015, 91 );

    m_OriginalTrainTrackData [3][185].Init ( 18311, -8880, 207, 5028, 155 );

    m_OriginalTrainTrackData [3][186].Init ( 18310, -8938, 205, 5050, 143 );

    m_OriginalTrainTrackData [3][187].Init ( 18312, -9100, 206, 5110, 232 );

    m_OriginalTrainTrackData [3][188].Init ( 18311, -9156, 205, 5131, 128 );

    m_OriginalTrainTrackData [3][189].Init ( 18311, -9196, 206, 5146, 223 );

    m_OriginalTrainTrackData [3][190].Init ( 18311, -9236, 205, 5161, 78 );

    m_OriginalTrainTrackData [3][191].Init ( 18311, -9292, 204, 5182, 90 );

    m_OriginalTrainTrackData [3][192].Init ( 18311, -9346, 202, 5203, 107 );

    m_OriginalTrainTrackData [3][193].Init ( 18311, -9384, 201, 5217, 232 );

    m_OriginalTrainTrackData [3][194].Init ( 18311, -9441, 198, 5238, 16 );

    m_OriginalTrainTrackData [3][195].Init ( 18311, -9488, 196, 5256, 141 );

    m_OriginalTrainTrackData [3][196].Init ( 18312, -9615, 188, 5304, 78 );

    m_OriginalTrainTrackData [3][197].Init ( 18312, -9744, 183, 5352, 21 );

    m_OriginalTrainTrackData [3][198].Init ( 18311, -9871, 183, 5400, 179 );

    m_OriginalTrainTrackData [3][199].Init ( 18311, -9920, 183, 5418, 2 );

    m_OriginalTrainTrackData [3][200].Init ( 18311, -10000, 183, 5448, 187 );

    m_OriginalTrainTrackData [3][201].Init ( 18311, -10069, 183, 5474, 140 );

    m_OriginalTrainTrackData [3][202].Init ( 18311, -10127, 183, 5496, 232 );

    m_OriginalTrainTrackData [3][203].Init ( 18311, -10195, 183, 5521, 21 );

    m_OriginalTrainTrackData [3][204].Init ( 18311, -10256, 183, 5544, 204 );

    m_OriginalTrainTrackData [3][205].Init ( 18311, -10323, 183, 5569, 104 );

    m_OriginalTrainTrackData [3][206].Init ( 18311, -10384, 183, 5592, 21 );

    m_OriginalTrainTrackData [3][207].Init ( 18311, -10455, 183, 5619, 12 );

    m_OriginalTrainTrackData [3][208].Init ( 18311, -10534, 183, 5648, 232 );

    m_OriginalTrainTrackData [3][209].Init ( 18311, -10640, 183, 5688, 21 );

    m_OriginalTrainTrackData [3][210].Init ( 18311, -10767, 183, 5736, 102 );

    m_OriginalTrainTrackData [3][211].Init ( 18311, -10806, 183, 5750, 181 );

    m_OriginalTrainTrackData [3][212].Init ( 18312, -10868, 183, 5773, 21 );

    m_OriginalTrainTrackData [3][213].Init ( 18312, -10952, 184, 5805, 166 );

    m_OriginalTrainTrackData [3][214].Init ( 18312, -11049, 184, 5841, 53 );

    m_OriginalTrainTrackData [3][215].Init ( 18312, -11095, 184, 5859, 21 );

    m_OriginalTrainTrackData [3][216].Init ( 18311, -11148, 183, 5878, 230 );

    m_OriginalTrainTrackData [3][217].Init ( 18310, -11192, 183, 5895, 181 );

    m_OriginalTrainTrackData [3][218].Init ( 18309, -11256, 182, 5919, 8 );

    m_OriginalTrainTrackData [3][219].Init ( 18309, -11297, 182, 5934, 115 );

    m_OriginalTrainTrackData [3][220].Init ( 18308, -11320, 182, 5943, 78 );

    m_OriginalTrainTrackData [3][221].Init ( 18302, -11381, 181, 5966, 226 );

    m_OriginalTrainTrackData [3][222].Init ( 18298, -11425, 181, 5983, 179 );

    m_OriginalTrainTrackData [3][223].Init ( 18295, -11462, 181, 5996, 2 );

    m_OriginalTrainTrackData [3][224].Init ( 18290, -11502, 180, 6012, 16 );

    m_OriginalTrainTrackData [3][225].Init ( 18284, -11553, 179, 6031, 142 );

    m_OriginalTrainTrackData [3][226].Init ( 18263, -11680, 177, 6079, 2 );

    m_OriginalTrainTrackData [3][227].Init ( 18251, -11737, 175, 6101, 40 );

    m_OriginalTrainTrackData [3][228].Init ( 18238, -11789, 173, 6121, 142 );

    m_OriginalTrainTrackData [3][229].Init ( 18227, -11832, 174, 6138, 130 );

    m_OriginalTrainTrackData [3][230].Init ( 18218, -11871, 172, 6153, 181 );

    m_OriginalTrainTrackData [3][231].Init ( 18208, -11911, 172, 6168, 167 );

    m_OriginalTrainTrackData [3][232].Init ( 18197, -11952, 170, 6184, 2 );

    m_OriginalTrainTrackData [3][233].Init ( 18165, -12050, 164, 6223, 16 );

    m_OriginalTrainTrackData [3][234].Init ( 18145, -12101, 162, 6243, 195 );

    m_OriginalTrainTrackData [3][235].Init ( 18129, -12140, 160, 6259, 53 );

    m_OriginalTrainTrackData [3][236].Init ( 18034, -12369, 149, 6352, 16 );

    m_OriginalTrainTrackData [3][237].Init ( 17984, -12488, 143, 6400, 115 );

    m_OriginalTrainTrackData [3][238].Init ( 17839, -12846, 125, 6545, 26 );

    m_OriginalTrainTrackData [3][239].Init ( 17790, -12958, 119, 6591, 222 );

    m_OriginalTrainTrackData [3][240].Init ( 17746, -13089, 115, 6643, 86 );

    m_OriginalTrainTrackData [3][241].Init ( 17720, -13182, 114, 6679, 130 );

    m_OriginalTrainTrackData [3][242].Init ( 17696, -13274, 110, 6715, 16 );

    m_OriginalTrainTrackData [3][243].Init ( 17667, -13380, 107, 6756, 31 );

    m_OriginalTrainTrackData [3][244].Init ( 17647, -13489, 103, 6798, 53 );

    m_OriginalTrainTrackData [3][245].Init ( 17631, -13622, 100, 6848, 16 );

    m_OriginalTrainTrackData [3][246].Init ( 17623, -13762, 98, 6900, 6 );

    m_OriginalTrainTrackData [3][247].Init ( 17623, -13858, 99, 6936, 53 );

    m_OriginalTrainTrackData [3][248].Init ( 17623, -13955, 97, 6973, 40 );

    m_OriginalTrainTrackData [3][249].Init ( 17622, -14085, 97, 7022, 178 );

    m_OriginalTrainTrackData [3][250].Init ( 17622, -14208, 97, 7068, 78 );

    m_OriginalTrainTrackData [3][251].Init ( 17622, -14342, 97, 7118, 40 );

    m_OriginalTrainTrackData [3][252].Init ( 17622, -14471, 97, 7166, 44 );

    m_OriginalTrainTrackData [3][253].Init ( 17622, -14595, 97, 7213, 142 );

    m_OriginalTrainTrackData [3][254].Init ( 17622, -14722, 97, 7260, 40 );

    m_OriginalTrainTrackData [3][255].Init ( 17621, -14867, 99, 7315, 44 );

    m_OriginalTrainTrackData [3][256].Init ( 17621, -14953, 100, 7347, 181 );

    m_OriginalTrainTrackData [3][257].Init ( 17622, -15037, 100, 7379, 168 );

    m_OriginalTrainTrackData [3][258].Init ( 17621, -15115, 101, 7408, 44 );

    m_OriginalTrainTrackData [3][259].Init ( 17621, -15195, 102, 7438, 78 );

    m_OriginalTrainTrackData [3][260].Init ( 17621, -15253, 100, 7460, 232 );

    m_OriginalTrainTrackData [3][261].Init ( 17615, -15312, 100, 7482, 172 );

    m_OriginalTrainTrackData [3][262].Init ( 17600, -15366, 100, 7503, 181 );

    m_OriginalTrainTrackData [3][263].Init ( 17578, -15419, 100, 7524, 232 );

    m_OriginalTrainTrackData [3][264].Init ( 17551, -15467, 100, 7545, 178 );

    m_OriginalTrainTrackData [3][265].Init ( 17516, -15514, 100, 7567, 181 );

    m_OriginalTrainTrackData [3][266].Init ( 17476, -15554, 100, 7588, 155 );

    m_OriginalTrainTrackData [3][267].Init ( 17430, -15589, 100, 7610, 167 );

    m_OriginalTrainTrackData [3][268].Init ( 17384, -15616, 100, 7630, 2 );

    m_OriginalTrainTrackData [3][269].Init ( 17330, -15639, 100, 7652, 142 );

    m_OriginalTrainTrackData [3][270].Init ( 17277, -15653, 100, 7672, 167 );

    m_OriginalTrainTrackData [3][271].Init ( 17222, -15662, 100, 7693, 2 );

    m_OriginalTrainTrackData [3][272].Init ( 17197, -15662, 100, 7703, 251 );

    m_OriginalTrainTrackData [3][273].Init ( 17125, -15662, 100, 7730, 167 );

    m_OriginalTrainTrackData [3][274].Init ( 17019, -15662, 100, 7769, 130 );

    m_OriginalTrainTrackData [3][275].Init ( 16887, -15663, 100, 7819, 7 );

    m_OriginalTrainTrackData [3][276].Init ( 16787, -15663, 100, 7856, 167 );

    m_OriginalTrainTrackData [3][277].Init ( 16698, -15663, 100, 7890, 27 );

    m_OriginalTrainTrackData [3][278].Init ( 16625, -15663, 100, 7917, 7 );

    m_OriginalTrainTrackData [3][279].Init ( 16559, -15663, 100, 7942, 166 );

    m_OriginalTrainTrackData [3][280].Init ( 16423, -15663, 100, 7993, 130 );

    m_OriginalTrainTrackData [3][281].Init ( 16207, -15663, 100, 8074, 7 );

    m_OriginalTrainTrackData [3][282].Init ( 15938, -15663, 100, 8175, 10 );

    m_OriginalTrainTrackData [3][283].Init ( 15725, -15663, 101, 8255, 27 );

    m_OriginalTrainTrackData [3][284].Init ( 15526, -15662, 100, 8329, 16 );

    m_OriginalTrainTrackData [3][285].Init ( 15337, -15662, 100, 8400, 49 );

    m_OriginalTrainTrackData [3][286].Init ( 15174, -15662, 100, 8461, 2 );

    m_OriginalTrainTrackData [3][287].Init ( 15012, -15663, 100, 8522, 16 );

    m_OriginalTrainTrackData [3][288].Init ( 14859, -15663, 100, 8579, 241 );

    m_OriginalTrainTrackData [3][289].Init ( 14735, -15663, 100, 8626, 27 );

    m_OriginalTrainTrackData [3][290].Init ( 14612, -15662, 100, 8672, 217 );

    m_OriginalTrainTrackData [3][291].Init ( 14536, -15662, 100, 8701, 166 );

    m_OriginalTrainTrackData [3][292].Init ( 14461, -15662, 100, 8729, 141 );

    m_OriginalTrainTrackData [3][293].Init ( 14338, -15662, 100, 8775, 63 );

    m_OriginalTrainTrackData [3][294].Init ( 14202, -15662, 100, 8826, 176 );

    m_OriginalTrainTrackData [3][295].Init ( 14044, -15662, 100, 8885, 32 );

    m_OriginalTrainTrackData [3][296].Init ( 13917, -15662, 100, 8933, 63 );

    m_OriginalTrainTrackData [3][297].Init ( 13670, -15663, 100, 9025, 172 );

    m_OriginalTrainTrackData [3][298].Init ( 13592, -15663, 100, 9055, 206 );

    m_OriginalTrainTrackData [3][299].Init ( 13479, -15663, 100, 9097, 40 );

    m_OriginalTrainTrackData [3][300].Init ( 13390, -15663, 100, 9130, 64 );

    m_OriginalTrainTrackData [3][301].Init ( 13311, -15663, 100, 9160, 79 );

    m_OriginalTrainTrackData [3][302].Init ( 13181, -15662, 100, 9209, 155 );

    m_OriginalTrainTrackData [3][303].Init ( 13063, -15662, 100, 9253, 243 );

    m_OriginalTrainTrackData [3][304].Init ( 12944, -15662, 100, 9298, 2 );

    m_OriginalTrainTrackData [3][305].Init ( 12826, -15661, 100, 9342, 84 );

    m_OriginalTrainTrackData [3][306].Init ( 12706, -15661, 100, 9387, 121 );

    m_OriginalTrainTrackData [3][307].Init ( 12589, -15661, 100, 9431, 130 );

    m_OriginalTrainTrackData [3][308].Init ( 12470, -15661, 100, 9475, 46 );

    m_OriginalTrainTrackData [3][309].Init ( 12351, -15661, 100, 9520, 140 );

    m_OriginalTrainTrackData [3][310].Init ( 12232, -15661, 100, 9565, 130 );

    m_OriginalTrainTrackData [3][311].Init ( 12114, -15661, 100, 9609, 138 );

    m_OriginalTrainTrackData [3][312].Init ( 11995, -15661, 100, 9653, 70 );

    m_OriginalTrainTrackData [3][313].Init ( 11877, -15662, 100, 9698, 117 );

    m_OriginalTrainTrackData [3][314].Init ( 11782, -15663, 100, 9733, 40 );

    m_OriginalTrainTrackData [3][315].Init ( 11685, -15663, 100, 9770, 172 );

    m_OriginalTrainTrackData [3][316].Init ( 11587, -15662, 100, 9806, 53 );

    m_OriginalTrainTrackData [3][317].Init ( 11490, -15658, 97, 9843, 40 );

    m_OriginalTrainTrackData [3][318].Init ( 11389, -15649, 94, 9881, 178 );

    m_OriginalTrainTrackData [3][319].Init ( 11286, -15636, 89, 9920, 181 );

    m_OriginalTrainTrackData [3][320].Init ( 11181, -15616, 81, 9960, 26 );

    m_OriginalTrainTrackData [3][321].Init ( 11072, -15590, 69, 10002, 51 );

    m_OriginalTrainTrackData [3][322].Init ( 10960, -15555, 59, 10046, 182 );

    m_OriginalTrainTrackData [3][323].Init ( 10844, -15510, 42, 10093, 18 );

    m_OriginalTrainTrackData [3][324].Init ( 10723, -15456, 20, 10142, 44 );

    m_OriginalTrainTrackData [3][325].Init ( 10658, -15418, 10, 10171, 192 );

    m_OriginalTrainTrackData [3][326].Init ( 10587, -15368, 0, 10203, 117 );

    m_OriginalTrainTrackData [3][327].Init ( 10512, -15306, -8, 10240, 48 );

    m_OriginalTrainTrackData [3][328].Init ( 10433, -15233, -16, 10280, 0 );

    m_OriginalTrainTrackData [3][329].Init ( 10349, -15152, -21, 10324, 116 );

    m_OriginalTrainTrackData [3][330].Init ( 10262, -15062, -28, 10371, 210 );

    m_OriginalTrainTrackData [3][331].Init ( 10173, -14964, -30, 10420, 115 );

    m_OriginalTrainTrackData [3][332].Init ( 10081, -14862, -32, 10472, 0 );

    m_OriginalTrainTrackData [3][333].Init ( 10032, -14805, -33, 10500, 15 );

    m_OriginalTrainTrackData [3][334].Init ( 9987, -14755, -35, 10525, 210 );

    m_OriginalTrainTrackData [3][335].Init ( 9876, -14628, -37, 10588, 64 );

    m_OriginalTrainTrackData [3][336].Init ( 9772, -14509, -40, 10648, 175 );

    m_OriginalTrainTrackData [3][337].Init ( 9663, -14382, -41, 10710, 115 );

    m_OriginalTrainTrackData [3][338].Init ( 9557, -14260, -42, 10771, 78 );

    m_OriginalTrainTrackData [3][339].Init ( 9446, -14131, -43, 10835, 227 );

    m_OriginalTrainTrackData [3][340].Init ( 9341, -14009, -43, 10895, 41 );

    m_OriginalTrainTrackData [3][341].Init ( 9235, -13885, -43, 10956, 134 );

    m_OriginalTrainTrackData [3][342].Init ( 9122, -13755, -43, 11021, 0 );

    m_OriginalTrainTrackData [3][343].Init ( 9011, -13624, -42, 11085, 115 );

    m_OriginalTrainTrackData [3][344].Init ( 8902, -13499, -42, 11148, 105 );

    m_OriginalTrainTrackData [3][345].Init ( 8779, -13358, -40, 11218, 86 );

    m_OriginalTrainTrackData [3][346].Init ( 8679, -13242, -40, 11275, 0 );

    m_OriginalTrainTrackData [3][347].Init ( 8561, -13106, -38, 11343, 98 );

    m_OriginalTrainTrackData [3][348].Init ( 8455, -12986, -37, 11403, 233 );

    m_OriginalTrainTrackData [3][349].Init ( 8342, -12856, -35, 11467, 64 );

    m_OriginalTrainTrackData [3][350].Init ( 8228, -12729, -34, 11531, 175 );

    m_OriginalTrainTrackData [3][351].Init ( 8133, -12624, -32, 11584, 152 );

    m_OriginalTrainTrackData [3][352].Init ( 7997, -12472, -31, 11661, 225 );

    m_OriginalTrainTrackData [3][353].Init ( 7879, -12342, -29, 11727, 132 );

    m_OriginalTrainTrackData [3][354].Init ( 7763, -12218, -27, 11790, 41 );

    m_OriginalTrainTrackData [3][355].Init ( 7664, -12112, -26, 11845, 25 );

    m_OriginalTrainTrackData [3][356].Init ( 7612, -12054, -26, 11874, 0 );

    m_OriginalTrainTrackData [3][357].Init ( 7526, -11965, -25, 11920, 5 );

    m_OriginalTrainTrackData [3][358].Init ( 7440, -11876, -24, 11967, 210 );

    m_OriginalTrainTrackData [3][359].Init ( 7366, -11797, -23, 12007, 115 );

    m_OriginalTrainTrackData [3][360].Init ( 7286, -11717, -22, 12050, 0 );

    m_OriginalTrainTrackData [3][361].Init ( 7217, -11648, -22, 12086, 46 );

    m_OriginalTrainTrackData [3][362].Init ( 7140, -11570, -21, 12128, 210 );

    m_OriginalTrainTrackData [3][363].Init ( 7041, -11470, -21, 12180, 64 );

    m_OriginalTrainTrackData [3][364].Init ( 6877, -11310, -20, 12266, 175 );

    m_OriginalTrainTrackData [3][365].Init ( 6792, -11228, -20, 12311, 155 );

    m_OriginalTrainTrackData [3][366].Init ( 6745, -11185, -20, 12334, 138 );

    m_OriginalTrainTrackData [3][367].Init ( 6670, -11116, -21, 12373, 160 );

    m_OriginalTrainTrackData [3][368].Init ( 6607, -11057, -21, 12405, 81 );

    m_OriginalTrainTrackData [3][369].Init ( 6542, -10995, -21, 12439, 197 );

    m_OriginalTrainTrackData [3][370].Init ( 6498, -10956, -21, 12461, 0 );

    m_OriginalTrainTrackData [3][371].Init ( 6414, -10880, -21, 12503, 36 );

    m_OriginalTrainTrackData [3][372].Init ( 6353, -10826, -20, 12534, 105 );

    m_OriginalTrainTrackData [3][373].Init ( 6292, -10770, -20, 12565, 86 );

    m_OriginalTrainTrackData [3][374].Init ( 6230, -10715, -20, 12596, 0 );

    m_OriginalTrainTrackData [3][375].Init ( 6192, -10681, -20, 12615, 154 );

    m_OriginalTrainTrackData [3][376].Init ( 6156, -10651, -19, 12633, 233 );

    m_OriginalTrainTrackData [3][377].Init ( 6072, -10579, -18, 12674, 64 );

    m_OriginalTrainTrackData [3][378].Init ( 6001, -10518, -16, 12709, 17 );

    m_OriginalTrainTrackData [3][379].Init ( 5937, -10463, -15, 12741, 202 );

    m_OriginalTrainTrackData [3][380].Init ( 5866, -10404, -14, 12775, 31 );

    m_OriginalTrainTrackData [3][381].Init ( 5820, -10364, -13, 12798, 160 );

    m_OriginalTrainTrackData [3][382].Init ( 5726, -10289, -10, 12843, 169 );

    m_OriginalTrainTrackData [3][383].Init ( 5665, -10241, -8, 12873, 56 );

    m_OriginalTrainTrackData [3][384].Init ( 5589, -10179, -5, 12909, 0 );

    m_OriginalTrainTrackData [3][385].Init ( 5524, -10128, -3, 12940, 36 );

    m_OriginalTrainTrackData [3][386].Init ( 5452, -10071, -1, 12975, 210 );

    m_OriginalTrainTrackData [3][387].Init ( 5369, -10008, 2, 13014, 115 );

    m_OriginalTrainTrackData [3][388].Init ( 5289, -9948, 5, 13051, 0 );

    m_OriginalTrainTrackData [3][389].Init ( 5171, -9861, 10, 13106, 72 );

    m_OriginalTrainTrackData [3][390].Init ( 5079, -9791, 14, 13150, 210 );

    m_OriginalTrainTrackData [3][391].Init ( 4947, -9700, 20, 13210, 117 );

    m_OriginalTrainTrackData [3][392].Init ( 4879, -9653, 24, 13241, 66 );

    m_OriginalTrainTrackData [3][393].Init ( 4791, -9593, 28, 13281, 116 );

    m_OriginalTrainTrackData [3][394].Init ( 4698, -9529, 32, 13323, 137 );

    m_OriginalTrainTrackData [3][395].Init ( 4645, -9495, 35, 13347, 11 );

    m_OriginalTrainTrackData [3][396].Init ( 4532, -9423, 41, 13397, 169 );

    m_OriginalTrainTrackData [3][397].Init ( 4412, -9350, 47, 13450, 185 );

    m_OriginalTrainTrackData [3][398].Init ( 4310, -9284, 53, 13495, 0 );

    m_OriginalTrainTrackData [3][399].Init ( 4178, -9208, 60, 13552, 36 );

    m_OriginalTrainTrackData [3][400].Init ( 4038, -9130, 67, 13612, 210 );

    m_OriginalTrainTrackData [3][401].Init ( 3915, -9060, 74, 13665, 115 );

    m_OriginalTrainTrackData [3][402].Init ( 3838, -9021, 78, 13698, 0 );

    m_OriginalTrainTrackData [3][403].Init ( 3711, -8958, 85, 13751, 0 );

    m_OriginalTrainTrackData [3][404].Init ( 3619, -8909, 90, 13790, 185 );

    m_OriginalTrainTrackData [3][405].Init ( 3511, -8856, 95, 13835, 186 );

    m_OriginalTrainTrackData [3][406].Init ( 3339, -8782, 104, 13905, 186 );

    m_OriginalTrainTrackData [3][407].Init ( 3208, -8725, 110, 13959, 0 );

    m_OriginalTrainTrackData [3][408].Init ( 3097, -8677, 116, 14004, 184 );

    m_OriginalTrainTrackData [3][409].Init ( 2780, -8561, 130, 14131, 186 );

    m_OriginalTrainTrackData [3][410].Init ( 2674, -8521, 134, 14173, 184 );

    m_OriginalTrainTrackData [3][411].Init ( 2607, -8502, 137, 14199, 0 );

    m_OriginalTrainTrackData [3][412].Init ( 2419, -8446, 143, 14273, 255 );

    m_OriginalTrainTrackData [3][413].Init ( 2240, -8392, 150, 14343, 187 );

    m_OriginalTrainTrackData [3][414].Init ( 2168, -8376, 152, 14371, 157 );

    m_OriginalTrainTrackData [3][415].Init ( 1985, -8335, 156, 14441, 0 );

    m_OriginalTrainTrackData [3][416].Init ( 1797, -8293, 161, 14513, 48 );

    m_OriginalTrainTrackData [3][417].Init ( 1674, -8274, 163, 14560, 159 );

    m_OriginalTrainTrackData [3][418].Init ( 1501, -8245, 166, 14626, 183 );

    m_OriginalTrainTrackData [3][419].Init ( 1341, -8221, 168, 14687, 0 );

    m_OriginalTrainTrackData [3][420].Init ( 1214, -8211, 168, 14734, 189 );

    m_OriginalTrainTrackData [3][421].Init ( 1026, -8194, 169, 14805, 156 );

    m_OriginalTrainTrackData [3][422].Init ( 876, -8182, 169, 14862, 144 );

    m_OriginalTrainTrackData [3][423].Init ( 784, -8181, 168, 14896, 0 );

    m_OriginalTrainTrackData [3][424].Init ( 636, -8179, 166, 14952, 133 );

    m_OriginalTrainTrackData [3][425].Init ( 459, -8177, 164, 15018, 110 );

    m_OriginalTrainTrackData [3][426].Init ( 370, -8176, 161, 15051, 107 );

    m_OriginalTrainTrackData [3][427].Init ( 209, -8176, 159, 15112, 0 );

    m_OriginalTrainTrackData [3][428].Init ( 21, -8176, 159, 15182, 105 );

    m_OriginalTrainTrackData [3][429].Init ( -166, -8176, 156, 15252, 108 );

    m_OriginalTrainTrackData [3][430].Init ( -355, -8176, 150, 15323, 112 );

    m_OriginalTrainTrackData [3][431].Init ( -542, -8176, 137, 15393, 0 );

    m_OriginalTrainTrackData [3][432].Init ( -637, -8176, 127, 15429, 107 );

    m_OriginalTrainTrackData [3][433].Init ( -729, -8176, 114, 15463, 110 );

    m_OriginalTrainTrackData [3][434].Init ( -850, -8181, 98, 15509, 111 );

    m_OriginalTrainTrackData [3][435].Init ( -973, -8195, 88, 15555, 0 );

    m_OriginalTrainTrackData [3][436].Init ( -1099, -8219, 82, 15603, 147 );

    m_OriginalTrainTrackData [3][437].Init ( -1226, -8250, 81, 15652, 121 );

    m_OriginalTrainTrackData [3][438].Init ( -1350, -8290, 84, 15701, 114 );

    m_OriginalTrainTrackData [3][439].Init ( -1471, -8337, 89, 15750, 0 );

    m_OriginalTrainTrackData [3][440].Init ( -1587, -8390, 98, 15798, 13 );

    m_OriginalTrainTrackData [3][441].Init ( -1695, -8450, 108, 15844, 143 );

    m_OriginalTrainTrackData [3][442].Init ( -1793, -8514, 120, 15888, 6 );

    m_OriginalTrainTrackData [3][443].Init ( -1881, -8582, 132, 15930, 0 );

    m_OriginalTrainTrackData [3][444].Init ( -1956, -8655, 146, 15969, 127 );

    m_OriginalTrainTrackData [3][445].Init ( -2016, -8730, 159, 16005, 127 );

    m_OriginalTrainTrackData [3][446].Init ( -2072, -8812, 172, 16042, 114 );

    m_OriginalTrainTrackData [3][447].Init ( -2133, -8900, 187, 16082, 0 );

    m_OriginalTrainTrackData [3][448].Init ( -2197, -8991, 202, 16124, 118 );

    m_OriginalTrainTrackData [3][449].Init ( -2265, -9086, 217, 16168, 109 );

    m_OriginalTrainTrackData [3][450].Init ( -2336, -9185, 233, 16214, 127 );

    m_OriginalTrainTrackData [3][451].Init ( -2406, -9280, 248, 16258, 0 );

    m_OriginalTrainTrackData [3][452].Init ( -2477, -9378, 263, 16303, 115 );

    m_OriginalTrainTrackData [3][453].Init ( -2547, -9470, 276, 16346, 117 );

    m_OriginalTrainTrackData [3][454].Init ( -2615, -9559, 289, 16388, 109 );

    m_OriginalTrainTrackData [3][455].Init ( -2680, -9642, 299, 16428, 0 );

    m_OriginalTrainTrackData [3][456].Init ( -2740, -9717, 308, 16464, 125 );

    m_OriginalTrainTrackData [3][457].Init ( -2795, -9785, 314, 16497, 127 );

    m_OriginalTrainTrackData [3][458].Init ( -2856, -9848, 319, 16530, 125 );

    m_OriginalTrainTrackData [3][459].Init ( -2932, -9906, 322, 16566, 0 );

    m_OriginalTrainTrackData [3][460].Init ( -3019, -9959, 325, 16604, 106 );

    m_OriginalTrainTrackData [3][461].Init ( -3116, -10005, 326, 16644, 110 );

    m_OriginalTrainTrackData [3][462].Init ( -3220, -10043, 328, 16686, 124 );

    m_OriginalTrainTrackData [3][463].Init ( -3330, -10070, 328, 16728, 0 );

    m_OriginalTrainTrackData [3][464].Init ( -3442, -10087, 329, 16771, 119 );

    m_OriginalTrainTrackData [3][465].Init ( -3556, -10092, 329, 16813, 106 );

    m_OriginalTrainTrackData [3][466].Init ( -3667, -10084, 328, 16855, 81 );

    m_OriginalTrainTrackData [3][467].Init ( -3774, -10058, 328, 16896, 0 );

    m_OriginalTrainTrackData [3][468].Init ( -3876, -10018, 328, 16937, 240 );

    m_OriginalTrainTrackData [3][469].Init ( -3969, -9958, 328, 16979, 155 );

    m_OriginalTrainTrackData [3][470].Init ( -4047, -9898, 328, 17016, 6 );

    m_OriginalTrainTrackData [3][471].Init ( -4111, -9848, 328, 17046, 0 );

    m_OriginalTrainTrackData [3][472].Init ( -4167, -9804, 328, 17073, 109 );

    m_OriginalTrainTrackData [3][473].Init ( -4218, -9766, 328, 17097, 87 );

    m_OriginalTrainTrackData [3][474].Init ( -4264, -9731, 328, 17119, 41 );

    m_OriginalTrainTrackData [3][475].Init ( -4311, -9695, 328, 17141, 0 );

    m_OriginalTrainTrackData [3][476].Init ( -4362, -9656, 328, 17165, 137 );

    m_OriginalTrainTrackData [3][477].Init ( -4418, -9611, 328, 17192, 131 );

    m_OriginalTrainTrackData [3][478].Init ( -4485, -9558, 328, 17224, 138 );

    m_OriginalTrainTrackData [3][479].Init ( -4563, -9492, 328, 17262, 0 );

    m_OriginalTrainTrackData [3][480].Init ( -4659, -9414, 328, 17308, 243 );

    m_OriginalTrainTrackData [3][481].Init ( -4773, -9319, 328, 17364, 173 );

    m_OriginalTrainTrackData [3][482].Init ( -4895, -9226, 331, 17422, 197 );

    m_OriginalTrainTrackData [3][483].Init ( -5015, -9148, 339, 17475, 0 );

    m_OriginalTrainTrackData [3][484].Init ( -5129, -9088, 351, 17524, 47 );

    m_OriginalTrainTrackData [3][485].Init ( -5241, -9043, 366, 17569, 46 );

    m_OriginalTrainTrackData [3][486].Init ( -5350, -9013, 384, 17611, 46 );

    m_OriginalTrainTrackData [3][487].Init ( -5454, -8999, 403, 17651, 0 );

    m_OriginalTrainTrackData [3][488].Init ( -5555, -8995, 422, 17689, 45 );

    m_OriginalTrainTrackData [3][489].Init ( -5652, -9006, 440, 17725, 40 );

    m_OriginalTrainTrackData [3][490].Init ( -5747, -9026, 457, 17762, 41 );

    m_OriginalTrainTrackData [3][491].Init ( -5838, -9057, 472, 17798, 0 );

    m_OriginalTrainTrackData [3][492].Init ( -5926, -9099, 482, 17834, 238 );

    m_OriginalTrainTrackData [3][493].Init ( -6010, -9149, 489, 17871, 203 );

    m_OriginalTrainTrackData [3][494].Init ( -6088, -9201, 492, 17906, 241 );

    m_OriginalTrainTrackData [3][495].Init ( -6158, -9253, 496, 17939, 0 );

    m_OriginalTrainTrackData [3][496].Init ( -6222, -9306, 501, 17970, 78 );

    m_OriginalTrainTrackData [3][497].Init ( -6280, -9362, 506, 18000, 79 );

    m_OriginalTrainTrackData [3][498].Init ( -6331, -9423, 513, 18030, 79 );

    m_OriginalTrainTrackData [3][499].Init ( -6377, -9491, 520, 18061, 0 );

    m_OriginalTrainTrackData [3][500].Init ( -6419, -9567, 529, 18093, 79 );

    m_OriginalTrainTrackData [3][501].Init ( -6455, -9656, 540, 18129, 80 );

    m_OriginalTrainTrackData [3][502].Init ( -6489, -9758, 553, 18170, 72 );

    m_OriginalTrainTrackData [3][503].Init ( -6521, -9875, 567, 18215, 0 );

    m_OriginalTrainTrackData [3][504].Init ( -6549, -10011, 583, 18267, 75 );

    m_OriginalTrainTrackData [3][505].Init ( -6574, -10168, 602, 18327, 77 );

    m_OriginalTrainTrackData [3][506].Init ( -6597, -10333, 622, 18389, 77 );

    m_OriginalTrainTrackData [3][507].Init ( -6619, -10495, 642, 18451, 0 );

    m_OriginalTrainTrackData [3][508].Init ( -6638, -10655, 661, 18511, 203 );

    m_OriginalTrainTrackData [3][509].Init ( -6660, -10810, 680, 18570, 198 );

    m_OriginalTrainTrackData [3][510].Init ( -6684, -10962, 697, 18627, 203 );

    m_OriginalTrainTrackData [3][511].Init ( -6712, -11105, 713, 18682, 0 );

    m_OriginalTrainTrackData [3][512].Init ( -6745, -11244, 728, 18736, 184 );

    m_OriginalTrainTrackData [3][513].Init ( -6784, -11374, 740, 18786, 182 );

    m_OriginalTrainTrackData [3][514].Init ( -6832, -11495, 750, 18835, 158 );

    m_OriginalTrainTrackData [3][515].Init ( -6889, -11605, 758, 18882, 0 );

    m_OriginalTrainTrackData [3][516].Init ( -6956, -11705, 762, 18927, 180 );

    m_OriginalTrainTrackData [3][517].Init ( -7036, -11792, 764, 18971, 34 );

    m_OriginalTrainTrackData [3][518].Init ( -7132, -11866, 761, 19017, 27 );

    m_OriginalTrainTrackData [3][519].Init ( -7242, -11920, 754, 19063, 0 );

    m_OriginalTrainTrackData [3][520].Init ( -7365, -11962, 741, 19111, 188 );

    m_OriginalTrainTrackData [3][521].Init ( -7498, -11991, 724, 19162, 19 );

    m_OriginalTrainTrackData [3][522].Init ( -7640, -12011, 703, 19216, 182 );

    m_OriginalTrainTrackData [3][523].Init ( -7791, -12025, 678, 19273, 0 );

    m_OriginalTrainTrackData [3][524].Init ( -7946, -12035, 651, 19331, 100 );

    m_OriginalTrainTrackData [3][525].Init ( -8105, -12044, 620, 19391, 101 );

    m_OriginalTrainTrackData [3][526].Init ( -8266, -12054, 586, 19451, 98 );

    m_OriginalTrainTrackData [3][527].Init ( -8428, -12067, 551, 19512, 0 );

    m_OriginalTrainTrackData [3][528].Init ( -8587, -12087, 513, 19573, 160 );

    m_OriginalTrainTrackData [3][529].Init ( -8744, -12116, 474, 19632, 180 );

    m_OriginalTrainTrackData [3][530].Init ( -8898, -12146, 436, 19691, 18 );

    m_OriginalTrainTrackData [3][531].Init ( -9055, -12165, 397, 19751, 0 );

    m_OriginalTrainTrackData [3][532].Init ( -9210, -12176, 360, 19809, 105 );

    m_OriginalTrainTrackData [3][533].Init ( -9366, -12176, 326, 19867, 101 );

    m_OriginalTrainTrackData [3][534].Init ( -9525, -12174, 293, 19927, 103 );

    m_OriginalTrainTrackData [3][535].Init ( -9685, -12166, 264, 19987, 0 );

    m_OriginalTrainTrackData [3][536].Init ( -9848, -12155, 238, 20048, 101 );

    m_OriginalTrainTrackData [3][537].Init ( -10018, -12144, 217, 20112, 102 );

    m_OriginalTrainTrackData [3][538].Init ( -10192, -12131, 199, 20178, 100 );

    m_OriginalTrainTrackData [3][539].Init ( -10371, -12119, 187, 20245, 0 );

    m_OriginalTrainTrackData [3][540].Init ( -10527, -12115, 181, 20303, 94 );

    m_OriginalTrainTrackData [3][541].Init ( -10630, -12113, 181, 20342, 94 );

    m_OriginalTrainTrackData [3][542].Init ( -10780, -12110, 181, 20398, 94 );

    m_OriginalTrainTrackData [3][543].Init ( -10952, -12108, 180, 20463, 0 );

    m_OriginalTrainTrackData [3][544].Init ( -11136, -12104, 176, 20532, 79 );

    m_OriginalTrainTrackData [3][545].Init ( -11340, -12101, 171, 20608, 79 );

    m_OriginalTrainTrackData [3][546].Init ( -11548, -12094, 164, 20686, 96 );

    m_OriginalTrainTrackData [3][547].Init ( -11759, -12085, 155, 20766, 0 );

    m_OriginalTrainTrackData [3][548].Init ( -11975, -12070, 147, 20847, 80 );

    m_OriginalTrainTrackData [3][549].Init ( -12189, -12051, 138, 20927, 96 );

    m_OriginalTrainTrackData [3][550].Init ( -12404, -12025, 129, 21009, 100 );

    m_OriginalTrainTrackData [3][551].Init ( -12616, -11990, 121, 21089, 0 );

    m_OriginalTrainTrackData [3][552].Init ( -12827, -11946, 114, 21170, 97 );

    m_OriginalTrainTrackData [3][553].Init ( -13032, -11893, 108, 21249, 95 );

    m_OriginalTrainTrackData [3][554].Init ( -13232, -11829, 104, 21328, 94 );

    m_OriginalTrainTrackData [3][555].Init ( -13422, -11754, 103, 21405, 0 );

    m_OriginalTrainTrackData [3][556].Init ( -13609, -11655, 103, 21484, 81 );

    m_OriginalTrainTrackData [3][557].Init ( -13792, -11534, 103, 21566, 121 );

    m_OriginalTrainTrackData [3][558].Init ( -13971, -11391, 103, 21652, 121 );

    m_OriginalTrainTrackData [3][559].Init ( -14144, -11228, 103, 21741, 0 );

    m_OriginalTrainTrackData [3][560].Init ( -14312, -11052, 103, 21833, 47 );

    m_OriginalTrainTrackData [3][561].Init ( -14472, -10867, 103, 21924, 30 );

    m_OriginalTrainTrackData [3][562].Init ( -14627, -10674, 103, 22017, 28 );

    m_OriginalTrainTrackData [3][563].Init ( -14775, -10474, 103, 22110, 0 );

    m_OriginalTrainTrackData [3][564].Init ( -14911, -10277, 103, 22200, 190 );

    m_OriginalTrainTrackData [3][565].Init ( -15039, -10082, 103, 22288, 86 );

    m_OriginalTrainTrackData [3][566].Init ( -15157, -9896, 103, 22370, 85 );

    m_OriginalTrainTrackData [3][567].Init ( -15262, -9718, 103, 22448, 0 );

    m_OriginalTrainTrackData [3][568].Init ( -15357, -9554, 103, 22519, 30 );

    m_OriginalTrainTrackData [3][569].Init ( -15441, -9400, 105, 22585, 30 );

    m_OriginalTrainTrackData [3][570].Init ( -15516, -9247, 109, 22649, 35 );

    m_OriginalTrainTrackData [3][571].Init ( -15582, -9097, 116, 22710, 0 );

    m_OriginalTrainTrackData [3][572].Init ( -15643, -8946, 124, 22771, 192 );

    m_OriginalTrainTrackData [3][573].Init ( -15694, -8792, 134, 22832, 84 );

    m_OriginalTrainTrackData [3][574].Init ( -15738, -8637, 145, 22892, 156 );

    m_OriginalTrainTrackData [3][575].Init ( -15775, -8477, 156, 22954, 0 );

    m_OriginalTrainTrackData [3][576].Init ( -15804, -8313, 166, 23016, 45 );

    m_OriginalTrainTrackData [3][577].Init ( -15828, -8144, 176, 23080, 191 );

    m_OriginalTrainTrackData [3][578].Init ( -15845, -7968, 184, 23147, 84 );

    m_OriginalTrainTrackData [3][579].Init ( -15857, -7782, 191, 23217, 0 );

    m_OriginalTrainTrackData [3][580].Init ( -15863, -7585, 196, 23290, 239 );

    m_OriginalTrainTrackData [3][581].Init ( -15861, -7382, 197, 23367, 113 );

    m_OriginalTrainTrackData [3][582].Init ( -15860, -7171, 197, 23446, 86 );

    m_OriginalTrainTrackData [3][583].Init ( -15860, -6966, 197, 23523, 0 );

    m_OriginalTrainTrackData [3][584].Init ( -15858, -6758, 197, 23601, 17 );

    m_OriginalTrainTrackData [3][585].Init ( -15858, -6552, 197, 23678, 21 );

    m_OriginalTrainTrackData [3][586].Init ( -15859, -6349, 197, 23754, 132 );

    m_OriginalTrainTrackData [3][587].Init ( -15860, -6144, 197, 23831, 0 );

    m_OriginalTrainTrackData [3][588].Init ( -15860, -5942, 197, 23907, 90 );

    m_OriginalTrainTrackData [3][589].Init ( -15862, -5739, 197, 23983, 96 );

    m_OriginalTrainTrackData [3][590].Init ( -15864, -5539, 197, 24058, 95 );

    m_OriginalTrainTrackData [3][591].Init ( -15866, -5339, 197, 24133, 0 );

    m_OriginalTrainTrackData [3][592].Init ( -15868, -5139, 197, 24208, 92 );

    m_OriginalTrainTrackData [3][593].Init ( -15869, -4936, 197, 24284, 20 );

    m_OriginalTrainTrackData [3][594].Init ( -15872, -4739, 197, 24358, 89 );

    m_OriginalTrainTrackData [3][595].Init ( -15872, -4640, 199, 24395, 0 );

    m_OriginalTrainTrackData [3][596].Init ( -15872, -4544, 197, 24431, 23 );

    m_OriginalTrainTrackData [3][597].Init ( -15873, -4416, 197, 24479, 23 );

    m_OriginalTrainTrackData [3][598].Init ( -15872, -4301, 197, 24522, 133 );

    m_OriginalTrainTrackData [3][599].Init ( -15871, -4160, 197, 24575, 0 );

    m_OriginalTrainTrackData [3][600].Init ( -15866, -4032, 197, 24623, 250 );

    m_OriginalTrainTrackData [3][601].Init ( -15859, -3904, 197, 24671, 133 );

    m_OriginalTrainTrackData [3][602].Init ( -15848, -3775, 197, 24720, 23 );

    m_OriginalTrainTrackData [3][603].Init ( -15835, -3649, 197, 24767, 0 );

    m_OriginalTrainTrackData [3][604].Init ( -15820, -3520, 197, 24816, 87 );

    m_OriginalTrainTrackData [3][605].Init ( -15806, -3392, 197, 24864, 87 );

    m_OriginalTrainTrackData [3][606].Init ( -15793, -3265, 197, 24912, 96 );

    m_OriginalTrainTrackData [3][607].Init ( -15780, -3136, 197, 24961, 0 );

    m_OriginalTrainTrackData [3][608].Init ( -15769, -3008, 197, 25009, 89 );

    m_OriginalTrainTrackData [3][609].Init ( -15761, -2880, 197, 25057, 87 );

    m_OriginalTrainTrackData [3][610].Init ( -15755, -2752, 197, 25105, 84 );

    m_OriginalTrainTrackData [3][611].Init ( -15744, -2624, 197, 25153, 0 );

    m_OriginalTrainTrackData [3][612].Init ( -15730, -2495, 197, 25202, 114 );

    m_OriginalTrainTrackData [3][613].Init ( -15718, -2410, 197, 25234, 109 );

    m_OriginalTrainTrackData [3][614].Init ( -15705, -2318, 197, 25269, 109 );

    m_OriginalTrainTrackData [3][615].Init ( -15690, -2220, 197, 25306, 0 );

    m_OriginalTrainTrackData [3][616].Init ( -15677, -2140, 197, 25336, 231 );

    m_OriginalTrainTrackData [3][617].Init ( -15663, -2053, 197, 25369, 135 );

    m_OriginalTrainTrackData [3][618].Init ( -15647, -1951, 197, 25408, 138 );

    m_OriginalTrainTrackData [3][619].Init ( -15639, -1883, 197, 25434, 0 );

    m_OriginalTrainTrackData [3][620].Init ( -15635, -1844, 197, 25448, 234 );

    m_OriginalTrainTrackData [3][621].Init ( -15622, -1728, 197, 25492, 134 );

    m_OriginalTrainTrackData [3][622].Init ( -15609, -1600, 197, 25540, 107 );

    m_OriginalTrainTrackData [3][623].Init ( -15603, -1522, 197, 25570, 0 );

    m_OriginalTrainTrackData [3][624].Init ( -15596, -1443, 197, 25600, 141 );

    m_OriginalTrainTrackData [3][625].Init ( -15590, -1343, 197, 25637, 7 );

    m_OriginalTrainTrackData [3][626].Init ( -15587, -1220, 197, 25683, 113 );

    m_OriginalTrainTrackData [3][627].Init ( -15587, -1064, 197, 25742, 0 );

    m_OriginalTrainTrackData [3][628].Init ( -15587, -867, 197, 25816, 150 );

    m_OriginalTrainTrackData [3][629].Init ( -15587, -670, 197, 25889, 53 );

    m_OriginalTrainTrackData [3][630].Init ( -15587, -459, 197, 25969, 60 );

    m_OriginalTrainTrackData [3][631].Init ( -15586, -298, 197, 26029, 0 );

    m_OriginalTrainTrackData [3][632].Init ( -15586, -161, 197, 26080, 58 );

    m_OriginalTrainTrackData [3][633].Init ( -15586, -32, 197, 26129, 56 );

    m_OriginalTrainTrackData [3][634].Init ( -15586, 87, 197, 26173, 213 );

    m_OriginalTrainTrackData [3][635].Init ( -15587, 164, 197, 26202, 0 );

    m_OriginalTrainTrackData [3][636].Init ( -15587, 199, 197, 26215, 27 );

    m_OriginalTrainTrackData [3][637].Init ( -15587, 229, 197, 26227, 227 );

    m_OriginalTrainTrackData [3][638].Init ( -15587, 289, 197, 26249, 132 );

    m_OriginalTrainTrackData [3][639].Init ( -15587, 309, 197, 26257, 0 );

    m_OriginalTrainTrackData [3][640].Init ( -15587, 327, 197, 26263, 29 );

    m_OriginalTrainTrackData [3][641].Init ( -15587, 371, 197, 26280, 60 );

    m_OriginalTrainTrackData [3][642].Init ( -15587, 430, 197, 26302, 53 );

    m_OriginalTrainTrackData [3][643].Init ( -15587, 482, 197, 26321, 0 );

    m_OriginalTrainTrackData [3][644].Init ( -15587, 540, 197, 26343, 55 );

    m_OriginalTrainTrackData [3][645].Init ( -15587, 590, 197, 26362, 57 );

    m_OriginalTrainTrackData [3][646].Init ( -15587, 631, 197, 26377, 215 );

    m_OriginalTrainTrackData [3][647].Init ( -15587, 679, 197, 26395, 0 );

    m_OriginalTrainTrackData [3][648].Init ( -15587, 736, 197, 26417, 9 );

    m_OriginalTrainTrackData [3][649].Init ( -15587, 799, 197, 26440, 216 );

    m_OriginalTrainTrackData [3][650].Init ( -15587, 857, 197, 26462, 29 );

    m_OriginalTrainTrackData [3][651].Init ( -15587, 909, 197, 26482, 0 );

    m_OriginalTrainTrackData [3][652].Init ( -15587, 958, 197, 26500, 8 );

    m_OriginalTrainTrackData [3][653].Init ( -15587, 1005, 197, 26518, 229 );

    m_OriginalTrainTrackData [3][654].Init ( -15586, 1024, 197, 26525, 216 );

    m_OriginalTrainTrackData [3][655].Init ( -15586, 1039, 197, 26530, 0 );

    m_OriginalTrainTrackData [3][0].Init ( 22324, -1657, 200, 0, 107 );

    m_OriginalTrainTrackData [3][1].Init ( 22312, -1681, 198, 10, 0 );

    m_OriginalTrainTrackData [3][2].Init ( 22246, -1800, 185, 61, 147 );

    m_OriginalTrainTrackData [3][3].Init ( 22183, -1919, 170, 111, 104 );

    m_OriginalTrainTrackData [3][4].Init ( 22131, -2020, 159, 154, 104 );

    m_OriginalTrainTrackData [3][5].Init ( 22099, -2077, 152, 178, 0 );

    m_OriginalTrainTrackData [3][6].Init ( 22070, -2125, 146, 199, 109 );

    m_OriginalTrainTrackData [3][7].Init ( 22025, -2178, 141, 225, 161 );

    m_OriginalTrainTrackData [3][8].Init ( 21973, -2223, 136, 251, 163 );

    m_OriginalTrainTrackData [3][9].Init ( 21930, -2246, 133, 269, 0 );

    m_OriginalTrainTrackData [3][10].Init ( 21847, -2275, 128, 302, 169 );

    m_OriginalTrainTrackData [3][11].Init ( 21774, -2300, 124, 331, 162 );

    m_OriginalTrainTrackData [3][12].Init ( 21672, -2327, 120, 371, 165 );

    m_OriginalTrainTrackData [3][13].Init ( 21568, -2353, 115, 411, 0 );

    m_OriginalTrainTrackData [3][14].Init ( 21476, -2370, 111, 446, 156 );

    m_OriginalTrainTrackData [3][15].Init ( 21382, -2381, 107, 482, 118 );

    m_OriginalTrainTrackData [3][16].Init ( 21290, -2391, 104, 516, 118 );

    m_OriginalTrainTrackData [3][17].Init ( 21192, -2395, 102, 553, 0 );

    m_OriginalTrainTrackData [3][18].Init ( 21094, -2392, 101, 590, 120 );

    m_OriginalTrainTrackData [3][19].Init ( 20998, -2389, 100, 626, 115 );

    m_OriginalTrainTrackData [3][20].Init ( 20897, -2381, 101, 664, 161 );

    m_OriginalTrainTrackData [3][21].Init ( 20808, -2373, 103, 697, 0 );

    m_OriginalTrainTrackData [3][22].Init ( 20736, -2365, 105, 725, 168 );

    m_OriginalTrainTrackData [3][23].Init ( 20665, -2356, 107, 751, 177 );

    m_OriginalTrainTrackData [3][24].Init ( 20596, -2344, 110, 778, 174 );

    m_OriginalTrainTrackData [3][25].Init ( 20528, -2332, 112, 804, 0 );

    m_OriginalTrainTrackData [3][26].Init ( 20463, -2322, 114, 828, 177 );

    m_OriginalTrainTrackData [3][27].Init ( 20396, -2311, 117, 854, 156 );

    m_OriginalTrainTrackData [3][28].Init ( 20326, -2301, 119, 880, 156 );

    m_OriginalTrainTrackData [3][29].Init ( 20259, -2292, 121, 906, 0 );

    m_OriginalTrainTrackData [3][30].Init ( 20198, -2284, 123, 929, 32 );

    m_OriginalTrainTrackData [3][31].Init ( 20085, -2271, 126, 971, 32 );

    m_OriginalTrainTrackData [3][32].Init ( 19910, -2250, 131, 1037, 25 );

    m_OriginalTrainTrackData [3][33].Init ( 19738, -2231, 135, 1102, 0 );

    m_OriginalTrainTrackData [3][34].Init ( 19620, -2223, 137, 1147, 38 );

    m_OriginalTrainTrackData [3][35].Init ( 19490, -2214, 139, 1195, 20 );

    m_OriginalTrainTrackData [3][36].Init ( 19378, -2207, 140, 1238, 39 );

    m_OriginalTrainTrackData [3][37].Init ( 19255, -2207, 140, 1284, 0 );

    m_OriginalTrainTrackData [3][38].Init ( 19153, -2215, 146, 1322, 39 );

    m_OriginalTrainTrackData [3][39].Init ( 19040, -2237, 151, 1365, 43 );

    m_OriginalTrainTrackData [3][40].Init ( 19001, -2251, 157, 1381, 43 );

    m_OriginalTrainTrackData [3][41].Init ( 18963, -2265, 162, 1396, 0 );

    m_OriginalTrainTrackData [3][42].Init ( 18940, -2274, 165, 1405, 43 );

    m_OriginalTrainTrackData [3][43].Init ( 18913, -2286, 169, 1416, 43 );

    m_OriginalTrainTrackData [3][44].Init ( 18839, -2317, 179, 1446, 32 );

    m_OriginalTrainTrackData [3][45].Init ( 18801, -2338, 187, 1463, 0 );

    m_OriginalTrainTrackData [3][46].Init ( 18746, -2368, 197, 1486, 32 );

    m_OriginalTrainTrackData [3][47].Init ( 18651, -2425, 216, 1528, 37 );

    m_OriginalTrainTrackData [3][48].Init ( 18561, -2487, 234, 1569, 31 );

    m_OriginalTrainTrackData [3][49].Init ( 18465, -2553, 256, 1612, 0 );

    m_OriginalTrainTrackData [3][50].Init ( 18376, -2612, 272, 1652, 49 );

    m_OriginalTrainTrackData [3][51].Init ( 18277, -2675, 290, 1696, 49 );

    m_OriginalTrainTrackData [3][52].Init ( 18189, -2727, 300, 1735, 44 );

    m_OriginalTrainTrackData [3][53].Init ( 18085, -2777, 312, 1778, 0 );

    m_OriginalTrainTrackData [3][54].Init ( 17995, -2814, 320, 1815, 50 );

    m_OriginalTrainTrackData [3][55].Init ( 17892, -2846, 330, 1855, 52 );

    m_OriginalTrainTrackData [3][56].Init ( 17809, -2865, 341, 1887, 53 );

    m_OriginalTrainTrackData [3][57].Init ( 17779, -2870, 345, 1898, 0 );

    m_OriginalTrainTrackData [3][58].Init ( 17714, -2883, 353, 1923, 52 );

    m_OriginalTrainTrackData [3][59].Init ( 17664, -2885, 361, 1942, 52 );

    m_OriginalTrainTrackData [3][60].Init ( 17580, -2893, 374, 1974, 30 );

    m_OriginalTrainTrackData [3][61].Init ( 17531, -2894, 382, 1992, 0 );

    m_OriginalTrainTrackData [3][62].Init ( 17453, -2894, 395, 2021, 62 );

    m_OriginalTrainTrackData [3][63].Init ( 17388, -2893, 406, 2046, 62 );

    m_OriginalTrainTrackData [3][64].Init ( 17321, -2891, 416, 2071, 63 );

    m_OriginalTrainTrackData [3][65].Init ( 17251, -2887, 426, 2097, 0 );

    m_OriginalTrainTrackData [3][66].Init ( 17195, -2885, 434, 2118, 28 );

    m_OriginalTrainTrackData [3][67].Init ( 17134, -2884, 440, 2141, 55 );

    m_OriginalTrainTrackData [3][68].Init ( 17060, -2883, 448, 2169, 56 );

    m_OriginalTrainTrackData [3][69].Init ( 16996, -2885, 454, 2193, 0 );

    m_OriginalTrainTrackData [3][70].Init ( 16911, -2889, 462, 2225, 20 );

    m_OriginalTrainTrackData [3][71].Init ( 16872, -2892, 466, 2239, 20 );

    m_OriginalTrainTrackData [3][72].Init ( 16798, -2900, 474, 2267, 26 );

    m_OriginalTrainTrackData [3][73].Init ( 16766, -2908, 478, 2280, 0 );

    m_OriginalTrainTrackData [3][74].Init ( 16702, -2922, 485, 2304, 192 );

    m_OriginalTrainTrackData [3][75].Init ( 16611, -2947, 496, 2339, 0 );

    m_OriginalTrainTrackData [3][76].Init ( 16517, -2982, 507, 2377, 0 );

    m_OriginalTrainTrackData [3][77].Init ( 16429, -3025, 516, 2414, 153 );

    m_OriginalTrainTrackData [3][78].Init ( 16345, -3076, 527, 2451, 0 );

    m_OriginalTrainTrackData [3][79].Init ( 16269, -3143, 534, 2489, 0 );

    m_OriginalTrainTrackData [3][80].Init ( 16196, -3212, 541, 2526, 46 );

    m_OriginalTrainTrackData [3][81].Init ( 16150, -3283, 543, 2558, 140 );

    m_OriginalTrainTrackData [3][82].Init ( 16129, -3319, 545, 2574, 155 );

    m_OriginalTrainTrackData [3][83].Init ( 16086, -3396, 547, 2607, 40 );

    m_OriginalTrainTrackData [3][84].Init ( 16070, -3441, 548, 2625, 102 );

    m_OriginalTrainTrackData [3][85].Init ( 16045, -3503, 549, 2650, 206 );

    m_OriginalTrainTrackData [3][86].Init ( 16015, -3610, 551, 2691, 46 );

    m_OriginalTrainTrackData [3][87].Init ( 15995, -3702, 555, 2727, 243 );

    m_OriginalTrainTrackData [3][88].Init ( 15981, -3799, 558, 2763, 130 );

    m_OriginalTrainTrackData [3][89].Init ( 15979, -3824, 559, 2773, 46 );

    m_OriginalTrainTrackData [3][90].Init ( 15977, -3912, 563, 2806, 166 );

    m_OriginalTrainTrackData [3][91].Init ( 15983, -3959, 565, 2824, 78 );

    m_OriginalTrainTrackData [3][92].Init ( 15987, -4016, 567, 2845, 46 );

    m_OriginalTrainTrackData [3][93].Init ( 15994, -4045, 568, 2856, 51 );

    m_OriginalTrainTrackData [3][94].Init ( 16016, -4128, 569, 2888, 2 );

    m_OriginalTrainTrackData [3][95].Init ( 16046, -4216, 571, 2923, 46 );

    m_OriginalTrainTrackData [3][96].Init ( 16069, -4258, 570, 2941, 230 );

    m_OriginalTrainTrackData [3][97].Init ( 16103, -4318, 568, 2967, 206 );

    m_OriginalTrainTrackData [3][98].Init ( 16122, -4347, 567, 2980, 46 );

    m_OriginalTrainTrackData [3][99].Init ( 16168, -4418, 566, 3012, 135 );

    m_OriginalTrainTrackData [3][100].Init ( 16201, -4457, 564, 3031, 214 );

    m_OriginalTrainTrackData [3][101].Init ( 16265, -4530, 560, 3067, 193 );

    m_OriginalTrainTrackData [3][102].Init ( 16339, -4603, 555, 3106, 118 );

    m_OriginalTrainTrackData [3][103].Init ( 16371, -4632, 552, 3123, 214 );

    m_OriginalTrainTrackData [3][104].Init ( 16442, -4694, 543, 3158, 231 );

    m_OriginalTrainTrackData [3][105].Init ( 16503, -4748, 536, 3188, 120 );

    m_OriginalTrainTrackData [3][106].Init ( 16576, -4812, 527, 3225, 232 );

    m_OriginalTrainTrackData [3][107].Init ( 16631, -4856, 519, 3251, 27 );

    m_OriginalTrainTrackData [3][108].Init ( 16704, -4920, 507, 3288, 129 );

    m_OriginalTrainTrackData [3][109].Init ( 16787, -4988, 494, 3328, 38 );

    m_OriginalTrainTrackData [3][110].Init ( 16830, -5023, 486, 3349, 28 );

    m_OriginalTrainTrackData [3][111].Init ( 16890, -5071, 476, 3378, 161 );

    m_OriginalTrainTrackData [3][112].Init ( 16935, -5107, 467, 3399, 182 );

    m_OriginalTrainTrackData [3][113].Init ( 16998, -5156, 456, 3429, 36 );

    m_OriginalTrainTrackData [3][114].Init ( 17031, -5184, 450, 3445, 67 );

    m_OriginalTrainTrackData [3][115].Init ( 17109, -5243, 435, 3482, 33 );

    m_OriginalTrainTrackData [3][116].Init ( 17181, -5298, 422, 3516, 38 );

    m_OriginalTrainTrackData [3][117].Init ( 17222, -5330, 414, 3535, 126 );

    m_OriginalTrainTrackData [3][118].Init ( 17292, -5381, 402, 3568, 123 );

    m_OriginalTrainTrackData [3][119].Init ( 17363, -5434, 390, 3601, 36 );

    m_OriginalTrainTrackData [3][120].Init ( 17448, -5499, 375, 3641, 126 );

    m_OriginalTrainTrackData [3][121].Init ( 17478, -5522, 371, 3655, 133 );

    m_OriginalTrainTrackData [3][122].Init ( 17543, -5571, 361, 3686, 28 );

    m_OriginalTrainTrackData [3][123].Init ( 17579, -5597, 356, 3703, 162 );

    m_OriginalTrainTrackData [3][124].Init ( 17644, -5644, 347, 3733, 133 );

    m_OriginalTrainTrackData [3][125].Init ( 17702, -5691, 337, 3761, 3 );

    m_OriginalTrainTrackData [3][126].Init ( 17753, -5732, 330, 3785, 160 );

    m_OriginalTrainTrackData [3][127].Init ( 17797, -5770, 323, 3807, 129 );

    m_OriginalTrainTrackData [3][128].Init ( 17828, -5798, 319, 3823, 14 );

    m_OriginalTrainTrackData [3][129].Init ( 17873, -5838, 313, 3845, 152 );

    m_OriginalTrainTrackData [3][130].Init ( 17906, -5871, 309, 3863, 252 );

    m_OriginalTrainTrackData [3][131].Init ( 17946, -5912, 304, 3884, 14 );

    m_OriginalTrainTrackData [3][132].Init ( 17989, -5968, 297, 3911, 49 );

    m_OriginalTrainTrackData [3][133].Init ( 18034, -6028, 290, 3939, 66 );

    m_OriginalTrainTrackData [3][134].Init ( 18072, -6085, 284, 3965, 192 );

    m_OriginalTrainTrackData [3][135].Init ( 18110, -6147, 277, 3992, 241 );

    m_OriginalTrainTrackData [3][136].Init ( 18137, -6204, 271, 4016, 2 );

    m_OriginalTrainTrackData [3][137].Init ( 18168, -6266, 265, 4042, 132 );

    m_OriginalTrainTrackData [3][138].Init ( 18190, -6330, 260, 4067, 86 );

    m_OriginalTrainTrackData [3][139].Init ( 18214, -6395, 254, 4093, 66 );

    m_OriginalTrainTrackData [3][140].Init ( 18234, -6463, 249, 4119, 132 );

    m_OriginalTrainTrackData [3][141].Init ( 18249, -6528, 244, 4144, 121 );

    m_OriginalTrainTrackData [3][142].Init ( 18262, -6596, 239, 4170, 117 );

    m_OriginalTrainTrackData [3][143].Init ( 18272, -6661, 234, 4195, 128 );

    m_OriginalTrainTrackData [3][144].Init ( 18282, -6722, 230, 4218, 69 );

    m_OriginalTrainTrackData [3][145].Init ( 18289, -6787, 226, 4243, 53 );

    m_OriginalTrainTrackData [3][146].Init ( 18295, -6845, 222, 4265, 128 );

    m_OriginalTrainTrackData [3][147].Init ( 18298, -6884, 220, 4279, 19 );

    m_OriginalTrainTrackData [3][148].Init ( 18300, -6944, 216, 4302, 78 );

    m_OriginalTrainTrackData [3][149].Init ( 18303, -6998, 213, 4322, 101 );

    m_OriginalTrainTrackData [3][150].Init ( 18305, -7049, 210, 4341, 71 );

    m_OriginalTrainTrackData [3][151].Init ( 18308, -7112, 206, 4365, 137 );

    m_OriginalTrainTrackData [3][152].Init ( 18310, -7138, 205, 4375, 128 );

    m_OriginalTrainTrackData [3][153].Init ( 18311, -7180, 202, 4390, 82 );

    m_OriginalTrainTrackData [3][154].Init ( 18311, -7227, 202, 4408, 179 );

    m_OriginalTrainTrackData [3][155].Init ( 18311, -7299, 203, 4435, 128 );

    m_OriginalTrainTrackData [3][156].Init ( 18311, -7342, 203, 4451, 57 );

    m_OriginalTrainTrackData [3][157].Init ( 18311, -7421, 203, 4481, 53 );

    m_OriginalTrainTrackData [3][158].Init ( 18311, -7491, 204, 4507, 133 );

    m_OriginalTrainTrackData [3][159].Init ( 18311, -7568, 204, 4536, 121 );

    m_OriginalTrainTrackData [3][160].Init ( 18311, -7636, 204, 4561, 130 );

    m_OriginalTrainTrackData [3][161].Init ( 18311, -7714, 205, 4591, 8 );

    m_OriginalTrainTrackData [3][162].Init ( 18311, -7768, 205, 4611, 121 );

    m_OriginalTrainTrackData [3][163].Init ( 18311, -7824, 205, 4632, 78 );

    m_OriginalTrainTrackData [3][164].Init ( 18311, -7902, 205, 4661, 187 );

    m_OriginalTrainTrackData [3][165].Init ( 18311, -7937, 205, 4674, 121 );

    m_OriginalTrainTrackData [3][166].Init ( 18311, -7980, 205, 4690, 130 );

    m_OriginalTrainTrackData [3][167].Init ( 18312, -8033, 206, 4710, 70 );

    m_OriginalTrainTrackData [3][168].Init ( 18311, -8121, 206, 4743, 121 );

    m_OriginalTrainTrackData [3][169].Init ( 18311, -8164, 206, 4759, 155 );

    m_OriginalTrainTrackData [3][170].Init ( 18311, -8179, 206, 4765, 187 );

    m_OriginalTrainTrackData [3][171].Init ( 18311, -8222, 206, 4781, 141 );

    m_OriginalTrainTrackData [3][172].Init ( 18311, -8264, 206, 4797, 53 );

    m_OriginalTrainTrackData [3][173].Init ( 18311, -8301, 206, 4811, 16 );

    m_OriginalTrainTrackData [3][174].Init ( 18311, -8340, 206, 4825, 70 );

    m_OriginalTrainTrackData [3][175].Init ( 18311, -8379, 206, 4840, 53 );

    m_OriginalTrainTrackData [3][176].Init ( 18311, -8413, 206, 4853, 128 );

    m_OriginalTrainTrackData [3][177].Init ( 18310, -8461, 206, 4871, 142 );

    m_OriginalTrainTrackData [3][178].Init ( 18310, -8516, 206, 4891, 130 );

    m_OriginalTrainTrackData [3][179].Init ( 18310, -8580, 207, 4915, 181 );

    m_OriginalTrainTrackData [3][180].Init ( 18310, -8639, 207, 4938, 142 );

    m_OriginalTrainTrackData [3][181].Init ( 18311, -8732, 207, 4972, 66 );

    m_OriginalTrainTrackData [3][182].Init ( 18310, -8753, 207, 4980, 142 );

    m_OriginalTrainTrackData [3][183].Init ( 18311, -8809, 205, 5001, 142 );

    m_OriginalTrainTrackData [3][184].Init ( 18311, -8845, 207, 5015, 91 );

    m_OriginalTrainTrackData [3][185].Init ( 18311, -8880, 207, 5028, 155 );

    m_OriginalTrainTrackData [3][186].Init ( 18310, -8938, 205, 5050, 143 );

    m_OriginalTrainTrackData [3][187].Init ( 18312, -9100, 206, 5110, 232 );

    m_OriginalTrainTrackData [3][188].Init ( 18311, -9156, 205, 5131, 128 );

    m_OriginalTrainTrackData [3][189].Init ( 18311, -9196, 206, 5146, 223 );

    m_OriginalTrainTrackData [3][190].Init ( 18311, -9236, 205, 5161, 78 );

    m_OriginalTrainTrackData [3][191].Init ( 18311, -9292, 204, 5182, 90 );

    m_OriginalTrainTrackData [3][192].Init ( 18311, -9346, 202, 5203, 107 );

    m_OriginalTrainTrackData [3][193].Init ( 18311, -9384, 201, 5217, 232 );

    m_OriginalTrainTrackData [3][194].Init ( 18311, -9441, 198, 5238, 16 );

    m_OriginalTrainTrackData [3][195].Init ( 18311, -9488, 196, 5256, 141 );

    m_OriginalTrainTrackData [3][196].Init ( 18312, -9615, 188, 5304, 78 );

    m_OriginalTrainTrackData [3][197].Init ( 18312, -9744, 183, 5352, 21 );

    m_OriginalTrainTrackData [3][198].Init ( 18311, -9871, 183, 5400, 179 );

    m_OriginalTrainTrackData [3][199].Init ( 18311, -9920, 183, 5418, 2 );

    m_OriginalTrainTrackData [3][200].Init ( 18311, -10000, 183, 5448, 187 );

    m_OriginalTrainTrackData [3][201].Init ( 18311, -10069, 183, 5474, 140 );

    m_OriginalTrainTrackData [3][202].Init ( 18311, -10127, 183, 5496, 232 );

    m_OriginalTrainTrackData [3][203].Init ( 18311, -10195, 183, 5521, 21 );

    m_OriginalTrainTrackData [3][204].Init ( 18311, -10256, 183, 5544, 204 );

    m_OriginalTrainTrackData [3][205].Init ( 18311, -10323, 183, 5569, 104 );

    m_OriginalTrainTrackData [3][206].Init ( 18311, -10384, 183, 5592, 21 );

    m_OriginalTrainTrackData [3][207].Init ( 18311, -10455, 183, 5619, 12 );

    m_OriginalTrainTrackData [3][208].Init ( 18311, -10534, 183, 5648, 232 );

    m_OriginalTrainTrackData [3][209].Init ( 18311, -10640, 183, 5688, 21 );

    m_OriginalTrainTrackData [3][210].Init ( 18311, -10767, 183, 5736, 102 );

    m_OriginalTrainTrackData [3][211].Init ( 18311, -10806, 183, 5750, 181 );

    m_OriginalTrainTrackData [3][212].Init ( 18312, -10868, 183, 5773, 21 );

    m_OriginalTrainTrackData [3][213].Init ( 18312, -10952, 184, 5805, 166 );

    m_OriginalTrainTrackData [3][214].Init ( 18312, -11049, 184, 5841, 53 );

    m_OriginalTrainTrackData [3][215].Init ( 18312, -11095, 184, 5859, 21 );

    m_OriginalTrainTrackData [3][216].Init ( 18311, -11148, 183, 5878, 230 );

    m_OriginalTrainTrackData [3][217].Init ( 18310, -11192, 183, 5895, 181 );

    m_OriginalTrainTrackData [3][218].Init ( 18309, -11256, 182, 5919, 8 );

    m_OriginalTrainTrackData [3][219].Init ( 18309, -11297, 182, 5934, 115 );

    m_OriginalTrainTrackData [3][220].Init ( 18308, -11320, 182, 5943, 78 );

    m_OriginalTrainTrackData [3][221].Init ( 18302, -11381, 181, 5966, 226 );

    m_OriginalTrainTrackData [3][222].Init ( 18298, -11425, 181, 5983, 179 );

    m_OriginalTrainTrackData [3][223].Init ( 18295, -11462, 181, 5996, 2 );

    m_OriginalTrainTrackData [3][224].Init ( 18290, -11502, 180, 6012, 16 );

    m_OriginalTrainTrackData [3][225].Init ( 18284, -11553, 179, 6031, 142 );

    m_OriginalTrainTrackData [3][226].Init ( 18263, -11680, 177, 6079, 2 );

    m_OriginalTrainTrackData [3][227].Init ( 18251, -11737, 175, 6101, 40 );

    m_OriginalTrainTrackData [3][228].Init ( 18238, -11789, 173, 6121, 142 );

    m_OriginalTrainTrackData [3][229].Init ( 18227, -11832, 174, 6138, 130 );

    m_OriginalTrainTrackData [3][230].Init ( 18218, -11871, 172, 6153, 181 );

    m_OriginalTrainTrackData [3][231].Init ( 18208, -11911, 172, 6168, 167 );

    m_OriginalTrainTrackData [3][232].Init ( 18197, -11952, 170, 6184, 2 );

    m_OriginalTrainTrackData [3][233].Init ( 18165, -12050, 164, 6223, 16 );

    m_OriginalTrainTrackData [3][234].Init ( 18145, -12101, 162, 6243, 195 );

    m_OriginalTrainTrackData [3][235].Init ( 18129, -12140, 160, 6259, 53 );

    m_OriginalTrainTrackData [3][236].Init ( 18034, -12369, 149, 6352, 16 );

    m_OriginalTrainTrackData [3][237].Init ( 17984, -12488, 143, 6400, 115 );

    m_OriginalTrainTrackData [3][238].Init ( 17839, -12846, 125, 6545, 26 );

    m_OriginalTrainTrackData [3][239].Init ( 17790, -12958, 119, 6591, 222 );

    m_OriginalTrainTrackData [3][240].Init ( 17746, -13089, 115, 6643, 86 );

    m_OriginalTrainTrackData [3][241].Init ( 17720, -13182, 114, 6679, 130 );

    m_OriginalTrainTrackData [3][242].Init ( 17696, -13274, 110, 6715, 16 );

    m_OriginalTrainTrackData [3][243].Init ( 17667, -13380, 107, 6756, 31 );

    m_OriginalTrainTrackData [3][244].Init ( 17647, -13489, 103, 6798, 53 );

    m_OriginalTrainTrackData [3][245].Init ( 17631, -13622, 100, 6848, 16 );

    m_OriginalTrainTrackData [3][246].Init ( 17623, -13762, 98, 6900, 6 );

    m_OriginalTrainTrackData [3][247].Init ( 17623, -13858, 99, 6936, 53 );

    m_OriginalTrainTrackData [3][248].Init ( 17623, -13955, 97, 6973, 40 );

    m_OriginalTrainTrackData [3][249].Init ( 17622, -14085, 97, 7022, 178 );

    m_OriginalTrainTrackData [3][250].Init ( 17622, -14208, 97, 7068, 78 );

    m_OriginalTrainTrackData [3][251].Init ( 17622, -14342, 97, 7118, 40 );

    m_OriginalTrainTrackData [3][252].Init ( 17622, -14471, 97, 7166, 44 );

    m_OriginalTrainTrackData [3][253].Init ( 17622, -14595, 97, 7213, 142 );

    m_OriginalTrainTrackData [3][254].Init ( 17622, -14722, 97, 7260, 40 );

    m_OriginalTrainTrackData [3][255].Init ( 17621, -14867, 99, 7315, 44 );

    m_OriginalTrainTrackData [3][256].Init ( 17621, -14953, 100, 7347, 181 );

    m_OriginalTrainTrackData [3][257].Init ( 17622, -15037, 100, 7379, 168 );

    m_OriginalTrainTrackData [3][258].Init ( 17621, -15115, 101, 7408, 44 );

    m_OriginalTrainTrackData [3][259].Init ( 17621, -15195, 102, 7438, 78 );

    m_OriginalTrainTrackData [3][260].Init ( 17621, -15253, 100, 7460, 232 );

    m_OriginalTrainTrackData [3][261].Init ( 17615, -15312, 100, 7482, 172 );

    m_OriginalTrainTrackData [3][262].Init ( 17600, -15366, 100, 7503, 181 );

    m_OriginalTrainTrackData [3][263].Init ( 17578, -15419, 100, 7524, 232 );

    m_OriginalTrainTrackData [3][264].Init ( 17551, -15467, 100, 7545, 178 );

    m_OriginalTrainTrackData [3][265].Init ( 17516, -15514, 100, 7567, 181 );

    m_OriginalTrainTrackData [3][266].Init ( 17476, -15554, 100, 7588, 155 );

    m_OriginalTrainTrackData [3][267].Init ( 17430, -15589, 100, 7610, 167 );

    m_OriginalTrainTrackData [3][268].Init ( 17384, -15616, 100, 7630, 2 );

    m_OriginalTrainTrackData [3][269].Init ( 17330, -15639, 100, 7652, 142 );

    m_OriginalTrainTrackData [3][270].Init ( 17277, -15653, 100, 7672, 167 );

    m_OriginalTrainTrackData [3][271].Init ( 17222, -15662, 100, 7693, 2 );

    m_OriginalTrainTrackData [3][272].Init ( 17197, -15662, 100, 7703, 251 );

    m_OriginalTrainTrackData [3][273].Init ( 17125, -15662, 100, 7730, 167 );

    m_OriginalTrainTrackData [3][274].Init ( 17019, -15662, 100, 7769, 130 );

    m_OriginalTrainTrackData [3][275].Init ( 16887, -15663, 100, 7819, 7 );

    m_OriginalTrainTrackData [3][276].Init ( 16787, -15663, 100, 7856, 167 );

    m_OriginalTrainTrackData [3][277].Init ( 16698, -15663, 100, 7890, 27 );

    m_OriginalTrainTrackData [3][278].Init ( 16625, -15663, 100, 7917, 7 );

    m_OriginalTrainTrackData [3][279].Init ( 16559, -15663, 100, 7942, 166 );

    m_OriginalTrainTrackData [3][280].Init ( 16423, -15663, 100, 7993, 130 );

    m_OriginalTrainTrackData [3][281].Init ( 16207, -15663, 100, 8074, 7 );

    m_OriginalTrainTrackData [3][282].Init ( 15938, -15663, 100, 8175, 10 );

    m_OriginalTrainTrackData [3][283].Init ( 15725, -15663, 101, 8255, 27 );

    m_OriginalTrainTrackData [3][284].Init ( 15526, -15662, 100, 8329, 16 );

    m_OriginalTrainTrackData [3][285].Init ( 15337, -15662, 100, 8400, 49 );

    m_OriginalTrainTrackData [3][286].Init ( 15174, -15662, 100, 8461, 2 );

    m_OriginalTrainTrackData [3][287].Init ( 15012, -15663, 100, 8522, 16 );

    m_OriginalTrainTrackData [3][288].Init ( 14859, -15663, 100, 8579, 241 );

    m_OriginalTrainTrackData [3][289].Init ( 14735, -15663, 100, 8626, 27 );

    m_OriginalTrainTrackData [3][290].Init ( 14612, -15662, 100, 8672, 217 );

    m_OriginalTrainTrackData [3][291].Init ( 14536, -15662, 100, 8701, 166 );

    m_OriginalTrainTrackData [3][292].Init ( 14461, -15662, 100, 8729, 141 );

    m_OriginalTrainTrackData [3][293].Init ( 14338, -15662, 100, 8775, 63 );

    m_OriginalTrainTrackData [3][294].Init ( 14202, -15662, 100, 8826, 176 );

    m_OriginalTrainTrackData [3][295].Init ( 14044, -15662, 100, 8885, 32 );

    m_OriginalTrainTrackData [3][296].Init ( 13917, -15662, 100, 8933, 63 );

    m_OriginalTrainTrackData [3][297].Init ( 13670, -15663, 100, 9025, 172 );

    m_OriginalTrainTrackData [3][298].Init ( 13592, -15663, 100, 9055, 206 );

    m_OriginalTrainTrackData [3][299].Init ( 13479, -15663, 100, 9097, 40 );

    m_OriginalTrainTrackData [3][300].Init ( 13390, -15663, 100, 9130, 64 );

    m_OriginalTrainTrackData [3][301].Init ( 13311, -15663, 100, 9160, 79 );

    m_OriginalTrainTrackData [3][302].Init ( 13181, -15662, 100, 9209, 155 );

    m_OriginalTrainTrackData [3][303].Init ( 13063, -15662, 100, 9253, 243 );

    m_OriginalTrainTrackData [3][304].Init ( 12944, -15662, 100, 9298, 2 );

    m_OriginalTrainTrackData [3][305].Init ( 12826, -15661, 100, 9342, 84 );

    m_OriginalTrainTrackData [3][306].Init ( 12706, -15661, 100, 9387, 121 );

    m_OriginalTrainTrackData [3][307].Init ( 12589, -15661, 100, 9431, 130 );

    m_OriginalTrainTrackData [3][308].Init ( 12470, -15661, 100, 9475, 46 );

    m_OriginalTrainTrackData [3][309].Init ( 12351, -15661, 100, 9520, 140 );

    m_OriginalTrainTrackData [3][310].Init ( 12232, -15661, 100, 9565, 130 );

    m_OriginalTrainTrackData [3][311].Init ( 12114, -15661, 100, 9609, 138 );

    m_OriginalTrainTrackData [3][312].Init ( 11995, -15661, 100, 9653, 70 );

    m_OriginalTrainTrackData [3][313].Init ( 11877, -15662, 100, 9698, 117 );

    m_OriginalTrainTrackData [3][314].Init ( 11782, -15663, 100, 9733, 40 );

    m_OriginalTrainTrackData [3][315].Init ( 11685, -15663, 100, 9770, 172 );

    m_OriginalTrainTrackData [3][316].Init ( 11587, -15662, 100, 9806, 53 );

    m_OriginalTrainTrackData [3][317].Init ( 11490, -15658, 97, 9843, 40 );

    m_OriginalTrainTrackData [3][318].Init ( 11389, -15649, 94, 9881, 178 );

    m_OriginalTrainTrackData [3][319].Init ( 11286, -15636, 89, 9920, 181 );

    m_OriginalTrainTrackData [3][320].Init ( 11181, -15616, 81, 9960, 26 );

    m_OriginalTrainTrackData [3][321].Init ( 11072, -15590, 69, 10002, 51 );

    m_OriginalTrainTrackData [3][322].Init ( 10960, -15555, 59, 10046, 182 );

    m_OriginalTrainTrackData [3][323].Init ( 10844, -15510, 42, 10093, 18 );

    m_OriginalTrainTrackData [3][324].Init ( 10723, -15456, 20, 10142, 44 );

    m_OriginalTrainTrackData [3][325].Init ( 10658, -15418, 10, 10171, 192 );

    m_OriginalTrainTrackData [3][326].Init ( 10587, -15368, 0, 10203, 117 );

    m_OriginalTrainTrackData [3][327].Init ( 10512, -15306, -8, 10240, 48 );

    m_OriginalTrainTrackData [3][328].Init ( 10433, -15233, -16, 10280, 0 );

    m_OriginalTrainTrackData [3][329].Init ( 10349, -15152, -21, 10324, 116 );

    m_OriginalTrainTrackData [3][330].Init ( 10262, -15062, -28, 10371, 210 );

    m_OriginalTrainTrackData [3][331].Init ( 10173, -14964, -30, 10420, 115 );

    m_OriginalTrainTrackData [3][332].Init ( 10081, -14862, -32, 10472, 0 );

    m_OriginalTrainTrackData [3][333].Init ( 10032, -14805, -33, 10500, 15 );

    m_OriginalTrainTrackData [3][334].Init ( 9987, -14755, -35, 10525, 210 );

    m_OriginalTrainTrackData [3][335].Init ( 9876, -14628, -37, 10588, 64 );

    m_OriginalTrainTrackData [3][336].Init ( 9772, -14509, -40, 10648, 175 );

    m_OriginalTrainTrackData [3][337].Init ( 9663, -14382, -41, 10710, 115 );

    m_OriginalTrainTrackData [3][338].Init ( 9557, -14260, -42, 10771, 78 );

    m_OriginalTrainTrackData [3][339].Init ( 9446, -14131, -43, 10835, 227 );

    m_OriginalTrainTrackData [3][340].Init ( 9341, -14009, -43, 10895, 41 );

    m_OriginalTrainTrackData [3][341].Init ( 9235, -13885, -43, 10956, 134 );

    m_OriginalTrainTrackData [3][342].Init ( 9122, -13755, -43, 11021, 0 );

    m_OriginalTrainTrackData [3][343].Init ( 9011, -13624, -42, 11085, 115 );

    m_OriginalTrainTrackData [3][344].Init ( 8902, -13499, -42, 11148, 105 );

    m_OriginalTrainTrackData [3][345].Init ( 8779, -13358, -40, 11218, 86 );

    m_OriginalTrainTrackData [3][346].Init ( 8679, -13242, -40, 11275, 0 );

    m_OriginalTrainTrackData [3][347].Init ( 8561, -13106, -38, 11343, 98 );

    m_OriginalTrainTrackData [3][348].Init ( 8455, -12986, -37, 11403, 233 );

    m_OriginalTrainTrackData [3][349].Init ( 8342, -12856, -35, 11467, 64 );

    m_OriginalTrainTrackData [3][350].Init ( 8228, -12729, -34, 11531, 175 );

    m_OriginalTrainTrackData [3][351].Init ( 8133, -12624, -32, 11584, 152 );

    m_OriginalTrainTrackData [3][352].Init ( 7997, -12472, -31, 11661, 225 );

    m_OriginalTrainTrackData [3][353].Init ( 7879, -12342, -29, 11727, 132 );

    m_OriginalTrainTrackData [3][354].Init ( 7763, -12218, -27, 11790, 41 );

    m_OriginalTrainTrackData [3][355].Init ( 7664, -12112, -26, 11845, 25 );

    m_OriginalTrainTrackData [3][356].Init ( 7612, -12054, -26, 11874, 0 );

    m_OriginalTrainTrackData [3][357].Init ( 7526, -11965, -25, 11920, 5 );

    m_OriginalTrainTrackData [3][358].Init ( 7440, -11876, -24, 11967, 210 );

    m_OriginalTrainTrackData [3][359].Init ( 7366, -11797, -23, 12007, 115 );

    m_OriginalTrainTrackData [3][360].Init ( 7286, -11717, -22, 12050, 0 );

    m_OriginalTrainTrackData [3][361].Init ( 7217, -11648, -22, 12086, 46 );

    m_OriginalTrainTrackData [3][362].Init ( 7140, -11570, -21, 12128, 210 );

    m_OriginalTrainTrackData [3][363].Init ( 7041, -11470, -21, 12180, 64 );

    m_OriginalTrainTrackData [3][364].Init ( 6877, -11310, -20, 12266, 175 );

    m_OriginalTrainTrackData [3][365].Init ( 6792, -11228, -20, 12311, 155 );

    m_OriginalTrainTrackData [3][366].Init ( 6745, -11185, -20, 12334, 138 );

    m_OriginalTrainTrackData [3][367].Init ( 6670, -11116, -21, 12373, 160 );

    m_OriginalTrainTrackData [3][368].Init ( 6607, -11057, -21, 12405, 81 );

    m_OriginalTrainTrackData [3][369].Init ( 6542, -10995, -21, 12439, 197 );

    m_OriginalTrainTrackData [3][370].Init ( 6498, -10956, -21, 12461, 0 );

    m_OriginalTrainTrackData [3][371].Init ( 6414, -10880, -21, 12503, 36 );

    m_OriginalTrainTrackData [3][372].Init ( 6353, -10826, -20, 12534, 105 );

    m_OriginalTrainTrackData [3][373].Init ( 6292, -10770, -20, 12565, 86 );

    m_OriginalTrainTrackData [3][374].Init ( 6230, -10715, -20, 12596, 0 );

    m_OriginalTrainTrackData [3][375].Init ( 6192, -10681, -20, 12615, 154 );

    m_OriginalTrainTrackData [3][376].Init ( 6156, -10651, -19, 12633, 233 );

    m_OriginalTrainTrackData [3][377].Init ( 6072, -10579, -18, 12674, 64 );

    m_OriginalTrainTrackData [3][378].Init ( 6001, -10518, -16, 12709, 17 );

    m_OriginalTrainTrackData [3][379].Init ( 5937, -10463, -15, 12741, 202 );

    m_OriginalTrainTrackData [3][380].Init ( 5866, -10404, -14, 12775, 31 );

    m_OriginalTrainTrackData [3][381].Init ( 5820, -10364, -13, 12798, 160 );

    m_OriginalTrainTrackData [3][382].Init ( 5726, -10289, -10, 12843, 169 );

    m_OriginalTrainTrackData [3][383].Init ( 5665, -10241, -8, 12873, 56 );

    m_OriginalTrainTrackData [3][384].Init ( 5589, -10179, -5, 12909, 0 );

    m_OriginalTrainTrackData [3][385].Init ( 5524, -10128, -3, 12940, 36 );

    m_OriginalTrainTrackData [3][386].Init ( 5452, -10071, -1, 12975, 210 );

    m_OriginalTrainTrackData [3][387].Init ( 5369, -10008, 2, 13014, 115 );

    m_OriginalTrainTrackData [3][388].Init ( 5289, -9948, 5, 13051, 0 );

    m_OriginalTrainTrackData [3][389].Init ( 5171, -9861, 10, 13106, 72 );

    m_OriginalTrainTrackData [3][390].Init ( 5079, -9791, 14, 13150, 210 );

    m_OriginalTrainTrackData [3][391].Init ( 4947, -9700, 20, 13210, 117 );

    m_OriginalTrainTrackData [3][392].Init ( 4879, -9653, 24, 13241, 66 );

    m_OriginalTrainTrackData [3][393].Init ( 4791, -9593, 28, 13281, 116 );

    m_OriginalTrainTrackData [3][394].Init ( 4698, -9529, 32, 13323, 137 );

    m_OriginalTrainTrackData [3][395].Init ( 4645, -9495, 35, 13347, 11 );

    m_OriginalTrainTrackData [3][396].Init ( 4532, -9423, 41, 13397, 169 );

    m_OriginalTrainTrackData [3][397].Init ( 4412, -9350, 47, 13450, 185 );

    m_OriginalTrainTrackData [3][398].Init ( 4310, -9284, 53, 13495, 0 );

    m_OriginalTrainTrackData [3][399].Init ( 4178, -9208, 60, 13552, 36 );

    m_OriginalTrainTrackData [3][400].Init ( 4038, -9130, 67, 13612, 210 );

    m_OriginalTrainTrackData [3][401].Init ( 3915, -9060, 74, 13665, 115 );

    m_OriginalTrainTrackData [3][402].Init ( 3838, -9021, 78, 13698, 0 );

    m_OriginalTrainTrackData [3][403].Init ( 3711, -8958, 85, 13751, 0 );

    m_OriginalTrainTrackData [3][404].Init ( 3619, -8909, 90, 13790, 185 );

    m_OriginalTrainTrackData [3][405].Init ( 3511, -8856, 95, 13835, 186 );

    m_OriginalTrainTrackData [3][406].Init ( 3339, -8782, 104, 13905, 186 );

    m_OriginalTrainTrackData [3][407].Init ( 3208, -8725, 110, 13959, 0 );

    m_OriginalTrainTrackData [3][408].Init ( 3097, -8677, 116, 14004, 184 );

    m_OriginalTrainTrackData [3][409].Init ( 2780, -8561, 130, 14131, 186 );

    m_OriginalTrainTrackData [3][410].Init ( 2674, -8521, 134, 14173, 184 );

    m_OriginalTrainTrackData [3][411].Init ( 2607, -8502, 137, 14199, 0 );

    m_OriginalTrainTrackData [3][412].Init ( 2419, -8446, 143, 14273, 255 );

    m_OriginalTrainTrackData [3][413].Init ( 2240, -8392, 150, 14343, 187 );

    m_OriginalTrainTrackData [3][414].Init ( 2168, -8376, 152, 14371, 157 );

    m_OriginalTrainTrackData [3][415].Init ( 1985, -8335, 156, 14441, 0 );

    m_OriginalTrainTrackData [3][416].Init ( 1797, -8293, 161, 14513, 48 );

    m_OriginalTrainTrackData [3][417].Init ( 1674, -8274, 163, 14560, 159 );

    m_OriginalTrainTrackData [3][418].Init ( 1501, -8245, 166, 14626, 183 );

    m_OriginalTrainTrackData [3][419].Init ( 1341, -8221, 168, 14687, 0 );

    m_OriginalTrainTrackData [3][420].Init ( 1214, -8211, 168, 14734, 189 );

    m_OriginalTrainTrackData [3][421].Init ( 1026, -8194, 169, 14805, 156 );

    m_OriginalTrainTrackData [3][422].Init ( 876, -8182, 169, 14862, 144 );

    m_OriginalTrainTrackData [3][423].Init ( 784, -8181, 168, 14896, 0 );

    m_OriginalTrainTrackData [3][424].Init ( 636, -8179, 166, 14952, 133 );

    m_OriginalTrainTrackData [3][425].Init ( 459, -8177, 164, 15018, 110 );

    m_OriginalTrainTrackData [3][426].Init ( 370, -8176, 161, 15051, 107 );

    m_OriginalTrainTrackData [3][427].Init ( 209, -8176, 159, 15112, 0 );

    m_OriginalTrainTrackData [3][428].Init ( 21, -8176, 159, 15182, 105 );

    m_OriginalTrainTrackData [3][429].Init ( -166, -8176, 156, 15252, 108 );

    m_OriginalTrainTrackData [3][430].Init ( -355, -8176, 150, 15323, 112 );

    m_OriginalTrainTrackData [3][431].Init ( -542, -8176, 137, 15393, 0 );

    m_OriginalTrainTrackData [3][432].Init ( -637, -8176, 127, 15429, 107 );

    m_OriginalTrainTrackData [3][433].Init ( -729, -8176, 114, 15463, 110 );

    m_OriginalTrainTrackData [3][434].Init ( -850, -8181, 98, 15509, 111 );

    m_OriginalTrainTrackData [3][435].Init ( -973, -8195, 88, 15555, 0 );

    m_OriginalTrainTrackData [3][436].Init ( -1099, -8219, 82, 15603, 147 );

    m_OriginalTrainTrackData [3][437].Init ( -1226, -8250, 81, 15652, 121 );

    m_OriginalTrainTrackData [3][438].Init ( -1350, -8290, 84, 15701, 114 );

    m_OriginalTrainTrackData [3][439].Init ( -1471, -8337, 89, 15750, 0 );

    m_OriginalTrainTrackData [3][440].Init ( -1587, -8390, 98, 15798, 13 );

    m_OriginalTrainTrackData [3][441].Init ( -1695, -8450, 108, 15844, 143 );

    m_OriginalTrainTrackData [3][442].Init ( -1793, -8514, 120, 15888, 6 );

    m_OriginalTrainTrackData [3][443].Init ( -1881, -8582, 132, 15930, 0 );

    m_OriginalTrainTrackData [3][444].Init ( -1956, -8655, 146, 15969, 127 );

    m_OriginalTrainTrackData [3][445].Init ( -2016, -8730, 159, 16005, 127 );

    m_OriginalTrainTrackData [3][446].Init ( -2072, -8812, 172, 16042, 114 );

    m_OriginalTrainTrackData [3][447].Init ( -2133, -8900, 187, 16082, 0 );

    m_OriginalTrainTrackData [3][448].Init ( -2197, -8991, 202, 16124, 118 );

    m_OriginalTrainTrackData [3][449].Init ( -2265, -9086, 217, 16168, 109 );

    m_OriginalTrainTrackData [3][450].Init ( -2336, -9185, 233, 16214, 127 );

    m_OriginalTrainTrackData [3][451].Init ( -2406, -9280, 248, 16258, 0 );

    m_OriginalTrainTrackData [3][452].Init ( -2477, -9378, 263, 16303, 115 );

    m_OriginalTrainTrackData [3][453].Init ( -2547, -9470, 276, 16346, 117 );

    m_OriginalTrainTrackData [3][454].Init ( -2615, -9559, 289, 16388, 109 );

    m_OriginalTrainTrackData [3][455].Init ( -2680, -9642, 299, 16428, 0 );

    m_OriginalTrainTrackData [3][456].Init ( -2740, -9717, 308, 16464, 125 );

    m_OriginalTrainTrackData [3][457].Init ( -2795, -9785, 314, 16497, 127 );

    m_OriginalTrainTrackData [3][458].Init ( -2856, -9848, 319, 16530, 125 );

    m_OriginalTrainTrackData [3][459].Init ( -2932, -9906, 322, 16566, 0 );

    m_OriginalTrainTrackData [3][460].Init ( -3019, -9959, 325, 16604, 106 );

    m_OriginalTrainTrackData [3][461].Init ( -3116, -10005, 326, 16644, 110 );

    m_OriginalTrainTrackData [3][462].Init ( -3220, -10043, 328, 16686, 124 );

    m_OriginalTrainTrackData [3][463].Init ( -3330, -10070, 328, 16728, 0 );

    m_OriginalTrainTrackData [3][464].Init ( -3442, -10087, 329, 16771, 119 );

    m_OriginalTrainTrackData [3][465].Init ( -3556, -10092, 329, 16813, 106 );

    m_OriginalTrainTrackData [3][466].Init ( -3667, -10084, 328, 16855, 81 );

    m_OriginalTrainTrackData [3][467].Init ( -3774, -10058, 328, 16896, 0 );

    m_OriginalTrainTrackData [3][468].Init ( -3876, -10018, 328, 16937, 240 );

    m_OriginalTrainTrackData [3][469].Init ( -3969, -9958, 328, 16979, 155 );

    m_OriginalTrainTrackData [3][470].Init ( -4047, -9898, 328, 17016, 6 );

    m_OriginalTrainTrackData [3][471].Init ( -4111, -9848, 328, 17046, 0 );

    m_OriginalTrainTrackData [3][472].Init ( -4167, -9804, 328, 17073, 109 );

    m_OriginalTrainTrackData [3][473].Init ( -4218, -9766, 328, 17097, 87 );

    m_OriginalTrainTrackData [3][474].Init ( -4264, -9731, 328, 17119, 41 );

    m_OriginalTrainTrackData [3][475].Init ( -4311, -9695, 328, 17141, 0 );

    m_OriginalTrainTrackData [3][476].Init ( -4362, -9656, 328, 17165, 137 );

    m_OriginalTrainTrackData [3][477].Init ( -4418, -9611, 328, 17192, 131 );

    m_OriginalTrainTrackData [3][478].Init ( -4485, -9558, 328, 17224, 138 );

    m_OriginalTrainTrackData [3][479].Init ( -4563, -9492, 328, 17262, 0 );

    m_OriginalTrainTrackData [3][480].Init ( -4659, -9414, 328, 17308, 243 );

    m_OriginalTrainTrackData [3][481].Init ( -4773, -9319, 328, 17364, 173 );

    m_OriginalTrainTrackData [3][482].Init ( -4895, -9226, 331, 17422, 197 );

    m_OriginalTrainTrackData [3][483].Init ( -5015, -9148, 339, 17475, 0 );

    m_OriginalTrainTrackData [3][484].Init ( -5129, -9088, 351, 17524, 47 );

    m_OriginalTrainTrackData [3][485].Init ( -5241, -9043, 366, 17569, 46 );

    m_OriginalTrainTrackData [3][486].Init ( -5350, -9013, 384, 17611, 46 );

    m_OriginalTrainTrackData [3][487].Init ( -5454, -8999, 403, 17651, 0 );

    m_OriginalTrainTrackData [3][488].Init ( -5555, -8995, 422, 17689, 45 );

    m_OriginalTrainTrackData [3][489].Init ( -5652, -9006, 440, 17725, 40 );

    m_OriginalTrainTrackData [3][490].Init ( -5747, -9026, 457, 17762, 41 );

    m_OriginalTrainTrackData [3][491].Init ( -5838, -9057, 472, 17798, 0 );

    m_OriginalTrainTrackData [3][492].Init ( -5926, -9099, 482, 17834, 238 );

    m_OriginalTrainTrackData [3][493].Init ( -6010, -9149, 489, 17871, 203 );

    m_OriginalTrainTrackData [3][494].Init ( -6088, -9201, 492, 17906, 241 );

    m_OriginalTrainTrackData [3][495].Init ( -6158, -9253, 496, 17939, 0 );

    m_OriginalTrainTrackData [3][496].Init ( -6222, -9306, 501, 17970, 78 );

    m_OriginalTrainTrackData [3][497].Init ( -6280, -9362, 506, 18000, 79 );

    m_OriginalTrainTrackData [3][498].Init ( -6331, -9423, 513, 18030, 79 );

    m_OriginalTrainTrackData [3][499].Init ( -6377, -9491, 520, 18061, 0 );

    m_OriginalTrainTrackData [3][500].Init ( -6419, -9567, 529, 18093, 79 );

    m_OriginalTrainTrackData [3][501].Init ( -6455, -9656, 540, 18129, 80 );

    m_OriginalTrainTrackData [3][502].Init ( -6489, -9758, 553, 18170, 72 );

    m_OriginalTrainTrackData [3][503].Init ( -6521, -9875, 567, 18215, 0 );

    m_OriginalTrainTrackData [3][504].Init ( -6549, -10011, 583, 18267, 75 );

    m_OriginalTrainTrackData [3][505].Init ( -6574, -10168, 602, 18327, 77 );

    m_OriginalTrainTrackData [3][506].Init ( -6597, -10333, 622, 18389, 77 );

    m_OriginalTrainTrackData [3][507].Init ( -6619, -10495, 642, 18451, 0 );

    m_OriginalTrainTrackData [3][508].Init ( -6638, -10655, 661, 18511, 203 );

    m_OriginalTrainTrackData [3][509].Init ( -6660, -10810, 680, 18570, 198 );

    m_OriginalTrainTrackData [3][510].Init ( -6684, -10962, 697, 18627, 203 );

    m_OriginalTrainTrackData [3][511].Init ( -6712, -11105, 713, 18682, 0 );

    m_OriginalTrainTrackData [3][512].Init ( -6745, -11244, 728, 18736, 184 );

    m_OriginalTrainTrackData [3][513].Init ( -6784, -11374, 740, 18786, 182 );

    m_OriginalTrainTrackData [3][514].Init ( -6832, -11495, 750, 18835, 158 );

    m_OriginalTrainTrackData [3][515].Init ( -6889, -11605, 758, 18882, 0 );

    m_OriginalTrainTrackData [3][516].Init ( -6956, -11705, 762, 18927, 180 );

    m_OriginalTrainTrackData [3][517].Init ( -7036, -11792, 764, 18971, 34 );

    m_OriginalTrainTrackData [3][518].Init ( -7132, -11866, 761, 19017, 27 );

    m_OriginalTrainTrackData [3][519].Init ( -7242, -11920, 754, 19063, 0 );

    m_OriginalTrainTrackData [3][520].Init ( -7365, -11962, 741, 19111, 188 );

    m_OriginalTrainTrackData [3][521].Init ( -7498, -11991, 724, 19162, 19 );

    m_OriginalTrainTrackData [3][522].Init ( -7640, -12011, 703, 19216, 182 );

    m_OriginalTrainTrackData [3][523].Init ( -7791, -12025, 678, 19273, 0 );

    m_OriginalTrainTrackData [3][524].Init ( -7946, -12035, 651, 19331, 100 );

    m_OriginalTrainTrackData [3][525].Init ( -8105, -12044, 620, 19391, 101 );

    m_OriginalTrainTrackData [3][526].Init ( -8266, -12054, 586, 19451, 98 );

    m_OriginalTrainTrackData [3][527].Init ( -8428, -12067, 551, 19512, 0 );

    m_OriginalTrainTrackData [3][528].Init ( -8587, -12087, 513, 19573, 160 );

    m_OriginalTrainTrackData [3][529].Init ( -8744, -12116, 474, 19632, 180 );

    m_OriginalTrainTrackData [3][530].Init ( -8898, -12146, 436, 19691, 18 );

    m_OriginalTrainTrackData [3][531].Init ( -9055, -12165, 397, 19751, 0 );

    m_OriginalTrainTrackData [3][532].Init ( -9210, -12176, 360, 19809, 105 );

    m_OriginalTrainTrackData [3][533].Init ( -9366, -12176, 326, 19867, 101 );

    m_OriginalTrainTrackData [3][534].Init ( -9525, -12174, 293, 19927, 103 );

    m_OriginalTrainTrackData [3][535].Init ( -9685, -12166, 264, 19987, 0 );

    m_OriginalTrainTrackData [3][536].Init ( -9848, -12155, 238, 20048, 101 );

    m_OriginalTrainTrackData [3][537].Init ( -10018, -12144, 217, 20112, 102 );

    m_OriginalTrainTrackData [3][538].Init ( -10192, -12131, 199, 20178, 100 );

    m_OriginalTrainTrackData [3][539].Init ( -10371, -12119, 187, 20245, 0 );

    m_OriginalTrainTrackData [3][540].Init ( -10527, -12115, 181, 20303, 94 );

    m_OriginalTrainTrackData [3][541].Init ( -10630, -12113, 181, 20342, 94 );

    m_OriginalTrainTrackData [3][542].Init ( -10780, -12110, 181, 20398, 94 );

    m_OriginalTrainTrackData [3][543].Init ( -10952, -12108, 180, 20463, 0 );

    m_OriginalTrainTrackData [3][544].Init ( -11136, -12104, 176, 20532, 79 );

    m_OriginalTrainTrackData [3][545].Init ( -11340, -12101, 171, 20608, 79 );

    m_OriginalTrainTrackData [3][546].Init ( -11548, -12094, 164, 20686, 96 );

    m_OriginalTrainTrackData [3][547].Init ( -11759, -12085, 155, 20766, 0 );

    m_OriginalTrainTrackData [3][548].Init ( -11975, -12070, 147, 20847, 80 );

    m_OriginalTrainTrackData [3][549].Init ( -12189, -12051, 138, 20927, 96 );

    m_OriginalTrainTrackData [3][550].Init ( -12404, -12025, 129, 21009, 100 );

    m_OriginalTrainTrackData [3][551].Init ( -12616, -11990, 121, 21089, 0 );

    m_OriginalTrainTrackData [3][552].Init ( -12827, -11946, 114, 21170, 97 );

    m_OriginalTrainTrackData [3][553].Init ( -13032, -11893, 108, 21249, 95 );

    m_OriginalTrainTrackData [3][554].Init ( -13232, -11829, 104, 21328, 94 );

    m_OriginalTrainTrackData [3][555].Init ( -13422, -11754, 103, 21405, 0 );

    m_OriginalTrainTrackData [3][556].Init ( -13609, -11655, 103, 21484, 81 );

    m_OriginalTrainTrackData [3][557].Init ( -13792, -11534, 103, 21566, 121 );

    m_OriginalTrainTrackData [3][558].Init ( -13971, -11391, 103, 21652, 121 );

    m_OriginalTrainTrackData [3][559].Init ( -14144, -11228, 103, 21741, 0 );

    m_OriginalTrainTrackData [3][560].Init ( -14312, -11052, 103, 21833, 47 );

    m_OriginalTrainTrackData [3][561].Init ( -14472, -10867, 103, 21924, 30 );

    m_OriginalTrainTrackData [3][562].Init ( -14627, -10674, 103, 22017, 28 );

    m_OriginalTrainTrackData [3][563].Init ( -14775, -10474, 103, 22110, 0 );

    m_OriginalTrainTrackData [3][564].Init ( -14911, -10277, 103, 22200, 190 );

    m_OriginalTrainTrackData [3][565].Init ( -15039, -10082, 103, 22288, 86 );

    m_OriginalTrainTrackData [3][566].Init ( -15157, -9896, 103, 22370, 85 );

    m_OriginalTrainTrackData [3][567].Init ( -15262, -9718, 103, 22448, 0 );

    m_OriginalTrainTrackData [3][568].Init ( -15357, -9554, 103, 22519, 30 );

    m_OriginalTrainTrackData [3][569].Init ( -15441, -9400, 105, 22585, 30 );

    m_OriginalTrainTrackData [3][570].Init ( -15516, -9247, 109, 22649, 35 );

    m_OriginalTrainTrackData [3][571].Init ( -15582, -9097, 116, 22710, 0 );

    m_OriginalTrainTrackData [3][572].Init ( -15643, -8946, 124, 22771, 192 );

    m_OriginalTrainTrackData [3][573].Init ( -15694, -8792, 134, 22832, 84 );

    m_OriginalTrainTrackData [3][574].Init ( -15738, -8637, 145, 22892, 156 );

    m_OriginalTrainTrackData [3][575].Init ( -15775, -8477, 156, 22954, 0 );

    m_OriginalTrainTrackData [3][576].Init ( -15804, -8313, 166, 23016, 45 );

    m_OriginalTrainTrackData [3][577].Init ( -15828, -8144, 176, 23080, 191 );

    m_OriginalTrainTrackData [3][578].Init ( -15845, -7968, 184, 23147, 84 );

    m_OriginalTrainTrackData [3][579].Init ( -15857, -7782, 191, 23217, 0 );

    m_OriginalTrainTrackData [3][580].Init ( -15863, -7585, 196, 23290, 239 );

    m_OriginalTrainTrackData [3][581].Init ( -15861, -7382, 197, 23367, 113 );

    m_OriginalTrainTrackData [3][582].Init ( -15860, -7171, 197, 23446, 86 );

    m_OriginalTrainTrackData [3][583].Init ( -15860, -6966, 197, 23523, 0 );

    m_OriginalTrainTrackData [3][584].Init ( -15858, -6758, 197, 23601, 17 );

    m_OriginalTrainTrackData [3][585].Init ( -15858, -6552, 197, 23678, 21 );

    m_OriginalTrainTrackData [3][586].Init ( -15859, -6349, 197, 23754, 132 );

    m_OriginalTrainTrackData [3][587].Init ( -15860, -6144, 197, 23831, 0 );

    m_OriginalTrainTrackData [3][588].Init ( -15860, -5942, 197, 23907, 90 );

    m_OriginalTrainTrackData [3][589].Init ( -15862, -5739, 197, 23983, 96 );

    m_OriginalTrainTrackData [3][590].Init ( -15864, -5539, 197, 24058, 95 );

    m_OriginalTrainTrackData [3][591].Init ( -15866, -5339, 197, 24133, 0 );

    m_OriginalTrainTrackData [3][592].Init ( -15868, -5139, 197, 24208, 92 );

    m_OriginalTrainTrackData [3][593].Init ( -15869, -4936, 197, 24284, 20 );

    m_OriginalTrainTrackData [3][594].Init ( -15872, -4739, 197, 24358, 89 );

    m_OriginalTrainTrackData [3][595].Init ( -15872, -4640, 199, 24395, 0 );

    m_OriginalTrainTrackData [3][596].Init ( -15872, -4544, 197, 24431, 23 );

    m_OriginalTrainTrackData [3][597].Init ( -15873, -4416, 197, 24479, 23 );

    m_OriginalTrainTrackData [3][598].Init ( -15872, -4301, 197, 24522, 133 );

    m_OriginalTrainTrackData [3][599].Init ( -15871, -4160, 197, 24575, 0 );

    m_OriginalTrainTrackData [3][600].Init ( -15866, -4032, 197, 24623, 250 );

    m_OriginalTrainTrackData [3][601].Init ( -15859, -3904, 197, 24671, 133 );

    m_OriginalTrainTrackData [3][602].Init ( -15848, -3775, 197, 24720, 23 );

    m_OriginalTrainTrackData [3][603].Init ( -15835, -3649, 197, 24767, 0 );

    m_OriginalTrainTrackData [3][604].Init ( -15820, -3520, 197, 24816, 87 );

    m_OriginalTrainTrackData [3][605].Init ( -15806, -3392, 197, 24864, 87 );

    m_OriginalTrainTrackData [3][606].Init ( -15793, -3265, 197, 24912, 96 );

    m_OriginalTrainTrackData [3][607].Init ( -15780, -3136, 197, 24961, 0 );

    m_OriginalTrainTrackData [3][608].Init ( -15769, -3008, 197, 25009, 89 );

    m_OriginalTrainTrackData [3][609].Init ( -15761, -2880, 197, 25057, 87 );

    m_OriginalTrainTrackData [3][610].Init ( -15755, -2752, 197, 25105, 84 );

    m_OriginalTrainTrackData [3][611].Init ( -15744, -2624, 197, 25153, 0 );

    m_OriginalTrainTrackData [3][612].Init ( -15730, -2495, 197, 25202, 114 );

    m_OriginalTrainTrackData [3][613].Init ( -15718, -2410, 197, 25234, 109 );

    m_OriginalTrainTrackData [3][614].Init ( -15705, -2318, 197, 25269, 109 );

    m_OriginalTrainTrackData [3][615].Init ( -15690, -2220, 197, 25306, 0 );

    m_OriginalTrainTrackData [3][616].Init ( -15677, -2140, 197, 25336, 231 );

    m_OriginalTrainTrackData [3][617].Init ( -15663, -2053, 197, 25369, 135 );

    m_OriginalTrainTrackData [3][618].Init ( -15647, -1951, 197, 25408, 138 );

    m_OriginalTrainTrackData [3][619].Init ( -15639, -1883, 197, 25434, 0 );

    m_OriginalTrainTrackData [3][620].Init ( -15635, -1844, 197, 25448, 234 );

    m_OriginalTrainTrackData [3][621].Init ( -15622, -1728, 197, 25492, 134 );

    m_OriginalTrainTrackData [3][622].Init ( -15609, -1600, 197, 25540, 107 );

    m_OriginalTrainTrackData [3][623].Init ( -15603, -1522, 197, 25570, 0 );

    m_OriginalTrainTrackData [3][624].Init ( -15596, -1443, 197, 25600, 141 );

    m_OriginalTrainTrackData [3][625].Init ( -15590, -1343, 197, 25637, 7 );

    m_OriginalTrainTrackData [3][626].Init ( -15587, -1220, 197, 25683, 113 );

    m_OriginalTrainTrackData [3][627].Init ( -15587, -1064, 197, 25742, 0 );

    m_OriginalTrainTrackData [3][628].Init ( -15587, -867, 197, 25816, 150 );

    m_OriginalTrainTrackData [3][629].Init ( -15587, -670, 197, 25889, 53 );

    m_OriginalTrainTrackData [3][630].Init ( -15587, -459, 197, 25969, 60 );

    m_OriginalTrainTrackData [3][631].Init ( -15586, -298, 197, 26029, 0 );

    m_OriginalTrainTrackData [3][632].Init ( -15586, -161, 197, 26080, 58 );

    m_OriginalTrainTrackData [3][633].Init ( -15586, -32, 197, 26129, 56 );

    m_OriginalTrainTrackData [3][634].Init ( -15586, 87, 197, 26173, 213 );

    m_OriginalTrainTrackData [3][635].Init ( -15587, 164, 197, 26202, 0 );

    m_OriginalTrainTrackData [3][636].Init ( -15587, 199, 197, 26215, 27 );

    m_OriginalTrainTrackData [3][637].Init ( -15587, 229, 197, 26227, 227 );

    m_OriginalTrainTrackData [3][638].Init ( -15587, 289, 197, 26249, 132 );

    m_OriginalTrainTrackData [3][639].Init ( -15587, 309, 197, 26257, 0 );

    m_OriginalTrainTrackData [3][640].Init ( -15587, 327, 197, 26263, 29 );

    m_OriginalTrainTrackData [3][641].Init ( -15587, 371, 197, 26280, 60 );

    m_OriginalTrainTrackData [3][642].Init ( -15587, 430, 197, 26302, 53 );

    m_OriginalTrainTrackData [3][643].Init ( -15587, 482, 197, 26321, 0 );

    m_OriginalTrainTrackData [3][644].Init ( -15587, 540, 197, 26343, 55 );

    m_OriginalTrainTrackData [3][645].Init ( -15587, 590, 197, 26362, 57 );

    m_OriginalTrainTrackData [3][646].Init ( -15587, 631, 197, 26377, 215 );

    m_OriginalTrainTrackData [3][647].Init ( -15587, 679, 197, 26395, 0 );

    m_OriginalTrainTrackData [3][648].Init ( -15587, 736, 197, 26417, 9 );

    m_OriginalTrainTrackData [3][649].Init ( -15587, 799, 197, 26440, 216 );

    m_OriginalTrainTrackData [3][650].Init ( -15587, 857, 197, 26462, 29 );

    m_OriginalTrainTrackData [3][651].Init ( -15587, 909, 197, 26482, 0 );

    m_OriginalTrainTrackData [3][652].Init ( -15587, 958, 197, 26500, 8 );

    m_OriginalTrainTrackData [3][653].Init ( -15587, 1005, 197, 26518, 229 );

    m_OriginalTrainTrackData [3][654].Init ( -15586, 1024, 197, 26525, 216 );

    m_OriginalTrainTrackData [3][655].Init ( -15586, 1039, 197, 26530, 0 );

    m_OriginalTrainTrackData [3][0].Init ( 22324, -1657, 200, 0, 107 );

    m_OriginalTrainTrackData [3][1].Init ( 22312, -1681, 198, 9, 0 );

    m_OriginalTrainTrackData [3][2].Init ( 22246, -1800, 185, 61, 147 );

    m_OriginalTrainTrackData [3][3].Init ( 22183, -1919, 170, 110, 104 );

    m_OriginalTrainTrackData [3][4].Init ( 22131, -2020, 159, 153, 104 );

    m_OriginalTrainTrackData [3][5].Init ( 22099, -2077, 152, 177, 0 );

    m_OriginalTrainTrackData [3][6].Init ( 22070, -2125, 146, 199, 109 );

    m_OriginalTrainTrackData [3][7].Init ( 22025, -2178, 141, 224, 161 );

    m_OriginalTrainTrackData [3][8].Init ( 21973, -2223, 136, 250, 163 );

    m_OriginalTrainTrackData [3][9].Init ( 21930, -2246, 133, 268, 0 );

    m_OriginalTrainTrackData [3][10].Init ( 21847, -2275, 128, 303, 169 );

    m_OriginalTrainTrackData [3][11].Init ( 21774, -2300, 124, 331, 162 );

    m_OriginalTrainTrackData [3][12].Init ( 21672, -2327, 120, 368, 165 );

    m_OriginalTrainTrackData [3][13].Init ( 21568, -2353, 115, 409, 0 );

    m_OriginalTrainTrackData [3][14].Init ( 21476, -2370, 111, 446, 156 );

    m_OriginalTrainTrackData [3][15].Init ( 21382, -2381, 107, 482, 118 );

    m_OriginalTrainTrackData [3][16].Init ( 21290, -2391, 104, 515, 118 );

    m_OriginalTrainTrackData [3][17].Init ( 21192, -2395, 102, 551, 0 );

    m_OriginalTrainTrackData [3][18].Init ( 21094, -2392, 101, 590, 120 );

    m_OriginalTrainTrackData [3][19].Init ( 20998, -2389, 100, 626, 115 );

    m_OriginalTrainTrackData [3][20].Init ( 20897, -2381, 101, 662, 161 );

    m_OriginalTrainTrackData [3][21].Init ( 20808, -2373, 103, 695, 0 );

    m_OriginalTrainTrackData [3][22].Init ( 20736, -2365, 105, 723, 168 );

    m_OriginalTrainTrackData [3][23].Init ( 20665, -2356, 107, 750, 177 );

    m_OriginalTrainTrackData [3][24].Init ( 20596, -2344, 110, 777, 174 );

    m_OriginalTrainTrackData [3][25].Init ( 20528, -2332, 112, 802, 0 );

    m_OriginalTrainTrackData [3][26].Init ( 20463, -2322, 114, 829, 177 );

    m_OriginalTrainTrackData [3][27].Init ( 20396, -2311, 117, 854, 156 );

    m_OriginalTrainTrackData [3][28].Init ( 20326, -2301, 119, 881, 156 );

    m_OriginalTrainTrackData [3][29].Init ( 20259, -2292, 121, 905, 0 );

    m_OriginalTrainTrackData [3][30].Init ( 20198, -2284, 123, 929, 32 );

    m_OriginalTrainTrackData [3][31].Init ( 20085, -2271, 126, 972, 32 );

    m_OriginalTrainTrackData [3][32].Init ( 19910, -2250, 131, 1038, 25 );

    m_OriginalTrainTrackData [3][33].Init ( 19738, -2231, 135, 1101, 0 );

    m_OriginalTrainTrackData [3][34].Init ( 19620, -2223, 137, 1147, 38 );

    m_OriginalTrainTrackData [3][35].Init ( 19490, -2214, 139, 1195, 20 );

    m_OriginalTrainTrackData [3][36].Init ( 19378, -2207, 140, 1237, 39 );

    m_OriginalTrainTrackData [3][37].Init ( 19255, -2207, 140, 1285, 0 );

    m_OriginalTrainTrackData [3][38].Init ( 19153, -2215, 146, 1321, 39 );

    m_OriginalTrainTrackData [3][39].Init ( 19040, -2237, 151, 1364, 43 );

    m_OriginalTrainTrackData [3][40].Init ( 19001, -2251, 157, 1380, 43 );

    m_OriginalTrainTrackData [3][41].Init ( 18963, -2265, 162, 1396, 0 );

    m_OriginalTrainTrackData [3][42].Init ( 18940, -2274, 165, 1406, 43 );

    m_OriginalTrainTrackData [3][43].Init ( 18913, -2286, 169, 1415, 43 );

    m_OriginalTrainTrackData [3][44].Init ( 18839, -2317, 179, 1447, 32 );

    m_OriginalTrainTrackData [3][45].Init ( 18801, -2338, 187, 1462, 0 );

    m_OriginalTrainTrackData [3][46].Init ( 18746, -2368, 197, 1487, 32 );

    m_OriginalTrainTrackData [3][47].Init ( 18651, -2425, 216, 1528, 37 );

    m_OriginalTrainTrackData [3][48].Init ( 18561, -2487, 234, 1567, 31 );

    m_OriginalTrainTrackData [3][49].Init ( 18465, -2553, 256, 1612, 0 );

    m_OriginalTrainTrackData [3][50].Init ( 18376, -2612, 272, 1652, 49 );

    m_OriginalTrainTrackData [3][51].Init ( 18277, -2675, 290, 1697, 49 );

    m_OriginalTrainTrackData [3][52].Init ( 18189, -2727, 300, 1735, 44 );

    m_OriginalTrainTrackData [3][53].Init ( 18085, -2777, 312, 1779, 0 );

    m_OriginalTrainTrackData [3][54].Init ( 17995, -2814, 320, 1814, 50 );

    m_OriginalTrainTrackData [3][55].Init ( 17892, -2846, 330, 1855, 52 );

    m_OriginalTrainTrackData [3][56].Init ( 17809, -2865, 341, 1886, 53 );

    m_OriginalTrainTrackData [3][57].Init ( 17779, -2870, 345, 1898, 0 );

    m_OriginalTrainTrackData [3][58].Init ( 17714, -2883, 353, 1923, 52 );

    m_OriginalTrainTrackData [3][59].Init ( 17664, -2885, 361, 1941, 52 );

    m_OriginalTrainTrackData [3][60].Init ( 17580, -2893, 374, 1974, 30 );

    m_OriginalTrainTrackData [3][61].Init ( 17531, -2894, 382, 1992, 0 );

    m_OriginalTrainTrackData [3][62].Init ( 17453, -2894, 395, 2022, 62 );

    m_OriginalTrainTrackData [3][63].Init ( 17388, -2893, 406, 2046, 62 );

    m_OriginalTrainTrackData [3][64].Init ( 17321, -2891, 416, 2070, 63 );

    m_OriginalTrainTrackData [3][65].Init ( 17251, -2887, 426, 2098, 0 );

    m_OriginalTrainTrackData [3][66].Init ( 17195, -2885, 434, 2119, 28 );

    m_OriginalTrainTrackData [3][67].Init ( 17134, -2884, 440, 2143, 55 );

    m_OriginalTrainTrackData [3][68].Init ( 17060, -2883, 448, 2170, 56 );

    m_OriginalTrainTrackData [3][69].Init ( 16996, -2885, 454, 2194, 0 );

    m_OriginalTrainTrackData [3][70].Init ( 16911, -2889, 462, 2227, 20 );

    m_OriginalTrainTrackData [3][71].Init ( 16872, -2892, 466, 2239, 20 );

    m_OriginalTrainTrackData [3][72].Init ( 16798, -2900, 474, 2269, 26 );

    m_OriginalTrainTrackData [3][73].Init ( 16766, -2908, 478, 2281, 0 );

    m_OriginalTrainTrackData [3][74].Init ( 16702, -2922, 485, 2306, 192 );

    m_OriginalTrainTrackData [3][75].Init ( 16611, -2947, 496, 2340, 0 );

    m_OriginalTrainTrackData [3][76].Init ( 16517, -2982, 507, 2378, 0 );

    m_OriginalTrainTrackData [3][77].Init ( 16429, -3025, 516, 2416, 153 );

    m_OriginalTrainTrackData [3][78].Init ( 16345, -3076, 527, 2451, 0 );

    m_OriginalTrainTrackData [3][79].Init ( 16269, -3143, 534, 2489, 0 );

    m_OriginalTrainTrackData [3][80].Init ( 16196, -3212, 541, 2527, 46 );

    m_OriginalTrainTrackData [3][81].Init ( 16150, -3283, 543, 2560, 140 );

    m_OriginalTrainTrackData [3][82].Init ( 16129, -3319, 545, 2573, 155 );

    m_OriginalTrainTrackData [3][83].Init ( 16086, -3396, 547, 2608, 40 );

    m_OriginalTrainTrackData [3][84].Init ( 16070, -3441, 548, 2627, 102 );

    m_OriginalTrainTrackData [3][85].Init ( 16045, -3503, 549, 2650, 206 );

    m_OriginalTrainTrackData [3][86].Init ( 16015, -3610, 551, 2694, 46 );

    m_OriginalTrainTrackData [3][87].Init ( 15995, -3702, 555, 2727, 243 );

    m_OriginalTrainTrackData [3][88].Init ( 15981, -3799, 558, 2764, 130 );

    m_OriginalTrainTrackData [3][89].Init ( 15979, -3824, 559, 2776, 46 );

    m_OriginalTrainTrackData [3][90].Init ( 15977, -3912, 563, 2809, 166 );

    m_OriginalTrainTrackData [3][91].Init ( 15983, -3959, 565, 2824, 78 );

    m_OriginalTrainTrackData [3][92].Init ( 15987, -4016, 567, 2848, 46 );

    m_OriginalTrainTrackData [3][93].Init ( 15994, -4045, 568, 2857, 51 );

    m_OriginalTrainTrackData [3][94].Init ( 16016, -4128, 569, 2892, 2 );

    m_OriginalTrainTrackData [3][95].Init ( 16046, -4216, 571, 2926, 46 );

    m_OriginalTrainTrackData [3][96].Init ( 16069, -4258, 570, 2943, 230 );

    m_OriginalTrainTrackData [3][97].Init ( 16103, -4318, 568, 2967, 206 );

    m_OriginalTrainTrackData [3][98].Init ( 16122, -4347, 567, 2982, 46 );

    m_OriginalTrainTrackData [3][99].Init ( 16168, -4418, 566, 3015, 135 );

    m_OriginalTrainTrackData [3][100].Init ( 16201, -4457, 564, 3034, 214 );

    m_OriginalTrainTrackData [3][101].Init ( 16265, -4530, 560, 3070, 193 );

    m_OriginalTrainTrackData [3][102].Init ( 16339, -4603, 555, 3108, 118 );

    m_OriginalTrainTrackData [3][103].Init ( 16371, -4632, 552, 3125, 214 );

    m_OriginalTrainTrackData [3][104].Init ( 16442, -4694, 543, 3160, 231 );

    m_OriginalTrainTrackData [3][105].Init ( 16503, -4748, 536, 3189, 120 );

    m_OriginalTrainTrackData [3][106].Init ( 16576, -4812, 527, 3228, 232 );

    m_OriginalTrainTrackData [3][107].Init ( 16631, -4856, 519, 3253, 27 );

    m_OriginalTrainTrackData [3][108].Init ( 16704, -4920, 507, 3292, 129 );

    m_OriginalTrainTrackData [3][109].Init ( 16787, -4988, 494, 3330, 38 );

    m_OriginalTrainTrackData [3][110].Init ( 16830, -5023, 486, 3349, 28 );

    m_OriginalTrainTrackData [3][111].Init ( 16890, -5071, 476, 3379, 161 );

    m_OriginalTrainTrackData [3][112].Init ( 16935, -5107, 467, 3400, 182 );

    m_OriginalTrainTrackData [3][113].Init ( 16998, -5156, 456, 3430, 36 );

    m_OriginalTrainTrackData [3][114].Init ( 17031, -5184, 450, 3447, 67 );

    m_OriginalTrainTrackData [3][115].Init ( 17109, -5243, 435, 3484, 33 );

    m_OriginalTrainTrackData [3][116].Init ( 17181, -5298, 422, 3518, 38 );

    m_OriginalTrainTrackData [3][117].Init ( 17222, -5330, 414, 3537, 126 );

    m_OriginalTrainTrackData [3][118].Init ( 17292, -5381, 402, 3570, 123 );

    m_OriginalTrainTrackData [3][119].Init ( 17363, -5434, 390, 3604, 36 );

    m_OriginalTrainTrackData [3][120].Init ( 17448, -5499, 375, 3645, 126 );

    m_OriginalTrainTrackData [3][121].Init ( 17478, -5522, 371, 3658, 133 );

    m_OriginalTrainTrackData [3][122].Init ( 17543, -5571, 361, 3688, 28 );

    m_OriginalTrainTrackData [3][123].Init ( 17579, -5597, 356, 3705, 162 );

    m_OriginalTrainTrackData [3][124].Init ( 17644, -5644, 347, 3735, 133 );

    m_OriginalTrainTrackData [3][125].Init ( 17702, -5691, 337, 3763, 3 );

    m_OriginalTrainTrackData [3][126].Init ( 17753, -5732, 330, 3789, 160 );

    m_OriginalTrainTrackData [3][127].Init ( 17797, -5770, 323, 3810, 129 );

    m_OriginalTrainTrackData [3][128].Init ( 17828, -5798, 319, 3825, 14 );

    m_OriginalTrainTrackData [3][129].Init ( 17873, -5838, 313, 3848, 152 );

    m_OriginalTrainTrackData [3][130].Init ( 17906, -5871, 309, 3865, 252 );

    m_OriginalTrainTrackData [3][131].Init ( 17946, -5912, 304, 3889, 14 );

    m_OriginalTrainTrackData [3][132].Init ( 17989, -5968, 297, 3914, 49 );

    m_OriginalTrainTrackData [3][133].Init ( 18034, -6028, 290, 3942, 66 );

    m_OriginalTrainTrackData [3][134].Init ( 18072, -6085, 284, 3968, 192 );

    m_OriginalTrainTrackData [3][135].Init ( 18110, -6147, 277, 3995, 241 );

    m_OriginalTrainTrackData [3][136].Init ( 18137, -6204, 271, 4019, 2 );

    m_OriginalTrainTrackData [3][137].Init ( 18168, -6266, 265, 4046, 132 );

    m_OriginalTrainTrackData [3][138].Init ( 18190, -6330, 260, 4070, 86 );

    m_OriginalTrainTrackData [3][139].Init ( 18214, -6395, 254, 4096, 66 );

    m_OriginalTrainTrackData [3][140].Init ( 18234, -6463, 249, 4122, 132 );

    m_OriginalTrainTrackData [3][141].Init ( 18249, -6528, 244, 4149, 121 );

    m_OriginalTrainTrackData [3][142].Init ( 18262, -6596, 239, 4174, 117 );

    m_OriginalTrainTrackData [3][143].Init ( 18272, -6661, 234, 4198, 128 );

    m_OriginalTrainTrackData [3][144].Init ( 18282, -6722, 230, 4222, 69 );

    m_OriginalTrainTrackData [3][145].Init ( 18289, -6787, 226, 4247, 53 );

    m_OriginalTrainTrackData [3][146].Init ( 18295, -6845, 222, 4268, 128 );

    m_OriginalTrainTrackData [3][147].Init ( 18298, -6884, 220, 4283, 19 );

    m_OriginalTrainTrackData [3][148].Init ( 18300, -6944, 216, 4307, 78 );

    m_OriginalTrainTrackData [3][149].Init ( 18303, -6998, 213, 4325, 101 );

    m_OriginalTrainTrackData [3][150].Init ( 18305, -7049, 210, 4346, 71 );

    m_OriginalTrainTrackData [3][151].Init ( 18308, -7112, 206, 4370, 137 );

    m_OriginalTrainTrackData [3][152].Init ( 18310, -7138, 205, 4379, 128 );

    m_OriginalTrainTrackData [3][153].Init ( 18311, -7180, 202, 4394, 82 );

    m_OriginalTrainTrackData [3][154].Init ( 18311, -7227, 202, 4412, 179 );

    m_OriginalTrainTrackData [3][155].Init ( 18311, -7299, 203, 4439, 128 );

    m_OriginalTrainTrackData [3][156].Init ( 18311, -7342, 203, 4454, 57 );

    m_OriginalTrainTrackData [3][157].Init ( 18311, -7421, 203, 4484, 53 );

    m_OriginalTrainTrackData [3][158].Init ( 18311, -7491, 204, 4511, 133 );

    m_OriginalTrainTrackData [3][159].Init ( 18311, -7568, 204, 4541, 121 );

    m_OriginalTrainTrackData [3][160].Init ( 18311, -7636, 204, 4565, 130 );

    m_OriginalTrainTrackData [3][161].Init ( 18311, -7714, 205, 4595, 8 );

    m_OriginalTrainTrackData [3][162].Init ( 18311, -7768, 205, 4616, 121 );

    m_OriginalTrainTrackData [3][163].Init ( 18311, -7824, 205, 4637, 78 );

    m_OriginalTrainTrackData [3][164].Init ( 18311, -7902, 205, 4664, 187 );

    m_OriginalTrainTrackData [3][165].Init ( 18311, -7937, 205, 4679, 121 );

    m_OriginalTrainTrackData [3][166].Init ( 18311, -7980, 205, 4694, 130 );

    m_OriginalTrainTrackData [3][167].Init ( 18312, -8033, 206, 4715, 70 );

    m_OriginalTrainTrackData [3][168].Init ( 18311, -8121, 206, 4749, 121 );

    m_OriginalTrainTrackData [3][169].Init ( 18311, -8164, 206, 4764, 155 );

    m_OriginalTrainTrackData [3][170].Init ( 18311, -8179, 206, 4770, 187 );

    m_OriginalTrainTrackData [3][171].Init ( 18311, -8222, 206, 4785, 141 );

    m_OriginalTrainTrackData [3][172].Init ( 18311, -8264, 206, 4803, 53 );

    m_OriginalTrainTrackData [3][173].Init ( 18311, -8301, 206, 4815, 16 );

    m_OriginalTrainTrackData [3][174].Init ( 18311, -8340, 206, 4830, 70 );

    m_OriginalTrainTrackData [3][175].Init ( 18311, -8379, 206, 4845, 53 );

    m_OriginalTrainTrackData [3][176].Init ( 18311, -8413, 206, 4857, 128 );

    m_OriginalTrainTrackData [3][177].Init ( 18310, -8461, 206, 4875, 142 );

    m_OriginalTrainTrackData [3][178].Init ( 18310, -8516, 206, 4896, 130 );

    m_OriginalTrainTrackData [3][179].Init ( 18310, -8580, 207, 4920, 181 );

    m_OriginalTrainTrackData [3][180].Init ( 18310, -8639, 207, 4941, 142 );

    m_OriginalTrainTrackData [3][181].Init ( 18311, -8732, 207, 4977, 66 );

    m_OriginalTrainTrackData [3][182].Init ( 18310, -8753, 207, 4986, 142 );

    m_OriginalTrainTrackData [3][183].Init ( 18311, -8809, 205, 5007, 142 );

    m_OriginalTrainTrackData [3][184].Init ( 18311, -8845, 207, 5019, 91 );

    m_OriginalTrainTrackData [3][185].Init ( 18311, -8880, 207, 5034, 155 );

    m_OriginalTrainTrackData [3][186].Init ( 18310, -8938, 205, 5055, 143 );

    m_OriginalTrainTrackData [3][187].Init ( 18312, -9100, 206, 5115, 232 );

    m_OriginalTrainTrackData [3][188].Init ( 18311, -9156, 205, 5136, 128 );

    m_OriginalTrainTrackData [3][189].Init ( 18311, -9196, 206, 5151, 223 );

    m_OriginalTrainTrackData [3][190].Init ( 18311, -9236, 205, 5166, 78 );

    m_OriginalTrainTrackData [3][191].Init ( 18311, -9292, 204, 5187, 90 );

    m_OriginalTrainTrackData [3][192].Init ( 18311, -9346, 202, 5208, 107 );

    m_OriginalTrainTrackData [3][193].Init ( 18311, -9384, 201, 5223, 232 );

    m_OriginalTrainTrackData [3][194].Init ( 18311, -9441, 198, 5244, 16 );

    m_OriginalTrainTrackData [3][195].Init ( 18311, -9488, 196, 5262, 141 );

    m_OriginalTrainTrackData [3][196].Init ( 18312, -9615, 188, 5307, 78 );

    m_OriginalTrainTrackData [3][197].Init ( 18312, -9744, 183, 5358, 21 );

    m_OriginalTrainTrackData [3][198].Init ( 18311, -9871, 183, 5403, 179 );

    m_OriginalTrainTrackData [3][199].Init ( 18311, -9920, 183, 5424, 2 );

    m_OriginalTrainTrackData [3][200].Init ( 18311, -10000, 183, 5454, 187 );

    m_OriginalTrainTrackData [3][201].Init ( 18311, -10069, 183, 5478, 140 );

    m_OriginalTrainTrackData [3][202].Init ( 18311, -10127, 183, 5499, 232 );

    m_OriginalTrainTrackData [3][203].Init ( 18311, -10195, 183, 5526, 21 );

    m_OriginalTrainTrackData [3][204].Init ( 18311, -10256, 183, 5550, 204 );

    m_OriginalTrainTrackData [3][205].Init ( 18311, -10323, 183, 5574, 104 );

    m_OriginalTrainTrackData [3][206].Init ( 18311, -10384, 183, 5598, 21 );

    m_OriginalTrainTrackData [3][207].Init ( 18311, -10455, 183, 5622, 12 );

    m_OriginalTrainTrackData [3][208].Init ( 18311, -10534, 183, 5652, 232 );

    m_OriginalTrainTrackData [3][209].Init ( 18311, -10640, 183, 5694, 21 );

    m_OriginalTrainTrackData [3][210].Init ( 18311, -10767, 183, 5739, 102 );

    m_OriginalTrainTrackData [3][211].Init ( 18311, -10806, 183, 5754, 181 );

    m_OriginalTrainTrackData [3][212].Init ( 18312, -10868, 183, 5778, 21 );

    m_OriginalTrainTrackData [3][213].Init ( 18312, -10952, 184, 5811, 166 );

    m_OriginalTrainTrackData [3][214].Init ( 18312, -11049, 184, 5847, 53 );

    m_OriginalTrainTrackData [3][215].Init ( 18312, -11095, 184, 5862, 21 );

    m_OriginalTrainTrackData [3][216].Init ( 18311, -11148, 183, 5883, 230 );

    m_OriginalTrainTrackData [3][217].Init ( 18310, -11192, 183, 5901, 181 );

    m_OriginalTrainTrackData [3][218].Init ( 18309, -11256, 182, 5925, 8 );

    m_OriginalTrainTrackData [3][219].Init ( 18309, -11297, 182, 5940, 115 );

    m_OriginalTrainTrackData [3][220].Init ( 18308, -11320, 182, 5949, 78 );

    m_OriginalTrainTrackData [3][221].Init ( 18302, -11381, 181, 5971, 226 );

    m_OriginalTrainTrackData [3][222].Init ( 18298, -11425, 181, 5989, 179 );

    m_OriginalTrainTrackData [3][223].Init ( 18295, -11462, 181, 6001, 2 );

    m_OriginalTrainTrackData [3][224].Init ( 18290, -11502, 180, 6016, 16 );

    m_OriginalTrainTrackData [3][225].Init ( 18284, -11553, 179, 6037, 142 );

    m_OriginalTrainTrackData [3][226].Init ( 18263, -11680, 177, 6086, 2 );

    m_OriginalTrainTrackData [3][227].Init ( 18251, -11737, 175, 6107, 40 );

    m_OriginalTrainTrackData [3][228].Init ( 18238, -11789, 173, 6126, 142 );

    m_OriginalTrainTrackData [3][229].Init ( 18227, -11832, 174, 6144, 130 );

    m_OriginalTrainTrackData [3][230].Init ( 18218, -11871, 172, 6157, 181 );

    m_OriginalTrainTrackData [3][231].Init ( 18208, -11911, 172, 6172, 167 );

    m_OriginalTrainTrackData [3][232].Init ( 18197, -11952, 170, 6191, 2 );

    m_OriginalTrainTrackData [3][233].Init ( 18165, -12050, 164, 6229, 16 );

    m_OriginalTrainTrackData [3][234].Init ( 18145, -12101, 162, 6248, 195 );

    m_OriginalTrainTrackData [3][235].Init ( 18129, -12140, 160, 6264, 53 );

    m_OriginalTrainTrackData [3][236].Init ( 18034, -12369, 149, 6358, 16 );

    m_OriginalTrainTrackData [3][237].Init ( 17984, -12488, 143, 6407, 115 );

    m_OriginalTrainTrackData [3][238].Init ( 17839, -12846, 125, 6551, 26 );

    m_OriginalTrainTrackData [3][239].Init ( 17790, -12958, 119, 6596, 222 );

    m_OriginalTrainTrackData [3][240].Init ( 17746, -13089, 115, 6649, 86 );

    m_OriginalTrainTrackData [3][241].Init ( 17720, -13182, 114, 6684, 130 );

    m_OriginalTrainTrackData [3][242].Init ( 17696, -13274, 110, 6721, 16 );

    m_OriginalTrainTrackData [3][243].Init ( 17667, -13380, 107, 6762, 31 );

    m_OriginalTrainTrackData [3][244].Init ( 17647, -13489, 103, 6805, 53 );

    m_OriginalTrainTrackData [3][245].Init ( 17631, -13622, 100, 6853, 16 );

    m_OriginalTrainTrackData [3][246].Init ( 17623, -13762, 98, 6907, 6 );

    m_OriginalTrainTrackData [3][247].Init ( 17623, -13858, 99, 6943, 53 );

    m_OriginalTrainTrackData [3][248].Init ( 17623, -13955, 97, 6979, 40 );

    m_OriginalTrainTrackData [3][249].Init ( 17622, -14085, 97, 7027, 178 );

    m_OriginalTrainTrackData [3][250].Init ( 17622, -14208, 97, 7075, 78 );

    m_OriginalTrainTrackData [3][251].Init ( 17622, -14342, 97, 7123, 40 );

    m_OriginalTrainTrackData [3][252].Init ( 17622, -14471, 97, 7171, 44 );

    m_OriginalTrainTrackData [3][253].Init ( 17622, -14595, 97, 7219, 142 );

    m_OriginalTrainTrackData [3][254].Init ( 17622, -14722, 97, 7267, 40 );

    m_OriginalTrainTrackData [3][255].Init ( 17621, -14867, 99, 7321, 44 );

    m_OriginalTrainTrackData [3][256].Init ( 17621, -14953, 100, 7354, 181 );

    m_OriginalTrainTrackData [3][257].Init ( 17622, -15037, 100, 7384, 168 );

    m_OriginalTrainTrackData [3][258].Init ( 17621, -15115, 101, 7414, 44 );

    m_OriginalTrainTrackData [3][259].Init ( 17621, -15195, 102, 7444, 78 );

    m_OriginalTrainTrackData [3][260].Init ( 17621, -15253, 100, 7465, 232 );

    m_OriginalTrainTrackData [3][261].Init ( 17615, -15312, 100, 7489, 172 );

    m_OriginalTrainTrackData [3][262].Init ( 17600, -15366, 100, 7507, 181 );

    m_OriginalTrainTrackData [3][263].Init ( 17578, -15419, 100, 7530, 232 );

    m_OriginalTrainTrackData [3][264].Init ( 17551, -15467, 100, 7552, 178 );

    m_OriginalTrainTrackData [3][265].Init ( 17516, -15514, 100, 7574, 181 );

    m_OriginalTrainTrackData [3][266].Init ( 17476, -15554, 100, 7595, 155 );

    m_OriginalTrainTrackData [3][267].Init ( 17430, -15589, 100, 7616, 167 );

    m_OriginalTrainTrackData [3][268].Init ( 17384, -15616, 100, 7636, 2 );

    m_OriginalTrainTrackData [3][269].Init ( 17330, -15639, 100, 7657, 142 );

    m_OriginalTrainTrackData [3][270].Init ( 17277, -15653, 100, 7679, 167 );

    m_OriginalTrainTrackData [3][271].Init ( 17222, -15662, 100, 7700, 2 );

    m_OriginalTrainTrackData [3][272].Init ( 17197, -15662, 100, 7709, 251 );

    m_OriginalTrainTrackData [3][273].Init ( 17125, -15662, 100, 7736, 167 );

    m_OriginalTrainTrackData [3][274].Init ( 17019, -15662, 100, 7775, 130 );

    m_OriginalTrainTrackData [3][275].Init ( 16887, -15663, 100, 7826, 7 );

    m_OriginalTrainTrackData [3][276].Init ( 16787, -15663, 100, 7862, 167 );

    m_OriginalTrainTrackData [3][277].Init ( 16698, -15663, 100, 7895, 27 );

    m_OriginalTrainTrackData [3][278].Init ( 16625, -15663, 100, 7922, 7 );

    m_OriginalTrainTrackData [3][279].Init ( 16559, -15663, 100, 7949, 166 );

    m_OriginalTrainTrackData [3][280].Init ( 16423, -15663, 100, 8000, 130 );

    m_OriginalTrainTrackData [3][281].Init ( 16207, -15663, 100, 8081, 7 );

    m_OriginalTrainTrackData [3][282].Init ( 15938, -15663, 100, 8180, 10 );

    m_OriginalTrainTrackData [3][283].Init ( 15725, -15663, 101, 8261, 27 );

    m_OriginalTrainTrackData [3][284].Init ( 15526, -15662, 100, 8336, 16 );

    m_OriginalTrainTrackData [3][285].Init ( 15337, -15662, 100, 8405, 49 );

    m_OriginalTrainTrackData [3][286].Init ( 15174, -15662, 100, 8468, 2 );

    m_OriginalTrainTrackData [3][287].Init ( 15012, -15663, 100, 8528, 16 );

    m_OriginalTrainTrackData [3][288].Init ( 14859, -15663, 100, 8585, 241 );

    m_OriginalTrainTrackData [3][289].Init ( 14735, -15663, 100, 8633, 27 );

    m_OriginalTrainTrackData [3][290].Init ( 14612, -15662, 100, 8678, 217 );

    m_OriginalTrainTrackData [3][291].Init ( 14536, -15662, 100, 8705, 166 );

    m_OriginalTrainTrackData [3][292].Init ( 14461, -15662, 100, 8735, 141 );

    m_OriginalTrainTrackData [3][293].Init ( 14338, -15662, 100, 8780, 63 );

    m_OriginalTrainTrackData [3][294].Init ( 14202, -15662, 100, 8831, 176 );

    m_OriginalTrainTrackData [3][295].Init ( 14044, -15662, 100, 8891, 32 );

    m_OriginalTrainTrackData [3][296].Init ( 13917, -15662, 100, 8939, 63 );

    m_OriginalTrainTrackData [3][297].Init ( 13670, -15663, 100, 9032, 172 );

    m_OriginalTrainTrackData [3][298].Init ( 13592, -15663, 100, 9059, 206 );

    m_OriginalTrainTrackData [3][299].Init ( 13479, -15663, 100, 9104, 40 );

    m_OriginalTrainTrackData [3][300].Init ( 13390, -15663, 100, 9137, 64 );

    m_OriginalTrainTrackData [3][301].Init ( 13311, -15663, 100, 9167, 79 );

    m_OriginalTrainTrackData [3][302].Init ( 13181, -15662, 100, 9215, 155 );

    m_OriginalTrainTrackData [3][303].Init ( 13063, -15662, 100, 9260, 243 );

    m_OriginalTrainTrackData [3][304].Init ( 12944, -15662, 100, 9302, 2 );

    m_OriginalTrainTrackData [3][305].Init ( 12826, -15661, 100, 9347, 84 );

    m_OriginalTrainTrackData [3][306].Init ( 12706, -15661, 100, 9392, 121 );

    m_OriginalTrainTrackData [3][307].Init ( 12589, -15661, 100, 9437, 130 );

    m_OriginalTrainTrackData [3][308].Init ( 12470, -15661, 100, 9482, 46 );

    m_OriginalTrainTrackData [3][309].Init ( 12351, -15661, 100, 9527, 140 );

    m_OriginalTrainTrackData [3][310].Init ( 12232, -15661, 100, 9569, 130 );

    m_OriginalTrainTrackData [3][311].Init ( 12114, -15661, 100, 9614, 138 );

    m_OriginalTrainTrackData [3][312].Init ( 11995, -15661, 100, 9659, 70 );

    m_OriginalTrainTrackData [3][313].Init ( 11877, -15662, 100, 9704, 117 );

    m_OriginalTrainTrackData [3][314].Init ( 11782, -15663, 100, 9740, 40 );

    m_OriginalTrainTrackData [3][315].Init ( 11685, -15663, 100, 9776, 172 );

    m_OriginalTrainTrackData [3][316].Init ( 11587, -15662, 100, 9812, 53 );

    m_OriginalTrainTrackData [3][317].Init ( 11490, -15658, 97, 9848, 40 );

    m_OriginalTrainTrackData [3][318].Init ( 11389, -15649, 94, 9888, 178 );

    m_OriginalTrainTrackData [3][319].Init ( 11286, -15636, 89, 9927, 181 );

    m_OriginalTrainTrackData [3][320].Init ( 11181, -15616, 81, 9967, 26 );

    m_OriginalTrainTrackData [3][321].Init ( 11072, -15590, 69, 10007, 51 );

    m_OriginalTrainTrackData [3][322].Init ( 10960, -15555, 59, 10051, 182 );

    m_OriginalTrainTrackData [3][323].Init ( 10844, -15510, 42, 10099, 18 );

    m_OriginalTrainTrackData [3][324].Init ( 10723, -15456, 20, 10148, 44 );

    m_OriginalTrainTrackData [3][325].Init ( 10658, -15418, 10, 10176, 192 );

    m_OriginalTrainTrackData [3][326].Init ( 10587, -15368, 0, 10209, 117 );

    m_OriginalTrainTrackData [3][327].Init ( 10512, -15306, -8, 10245, 48 );

    m_OriginalTrainTrackData [3][328].Init ( 10433, -15233, -16, 10285, 0 );

    m_OriginalTrainTrackData [3][329].Init ( 10349, -15152, -21, 10330, 116 );

    m_OriginalTrainTrackData [3][330].Init ( 10262, -15062, -28, 10379, 210 );

    m_OriginalTrainTrackData [3][331].Init ( 10173, -14964, -30, 10427, 115 );

    m_OriginalTrainTrackData [3][332].Init ( 10081, -14862, -32, 10479, 0 );

    m_OriginalTrainTrackData [3][333].Init ( 10032, -14805, -33, 10506, 15 );

    m_OriginalTrainTrackData [3][334].Init ( 9987, -14755, -35, 10532, 210 );

    m_OriginalTrainTrackData [3][335].Init ( 9876, -14628, -37, 10595, 64 );

    m_OriginalTrainTrackData [3][336].Init ( 9772, -14509, -40, 10655, 175 );

    m_OriginalTrainTrackData [3][337].Init ( 9663, -14382, -41, 10719, 115 );

    m_OriginalTrainTrackData [3][338].Init ( 9557, -14260, -42, 10778, 78 );

    m_OriginalTrainTrackData [3][339].Init ( 9446, -14131, -43, 10842, 227 );

    m_OriginalTrainTrackData [3][340].Init ( 9341, -14009, -43, 10902, 41 );

    m_OriginalTrainTrackData [3][341].Init ( 9235, -13885, -43, 10963, 134 );

    m_OriginalTrainTrackData [3][342].Init ( 9122, -13755, -43, 11027, 0 );

    m_OriginalTrainTrackData [3][343].Init ( 9011, -13624, -42, 11091, 115 );

    m_OriginalTrainTrackData [3][344].Init ( 8902, -13499, -42, 11155, 105 );

    m_OriginalTrainTrackData [3][345].Init ( 8779, -13358, -40, 11225, 86 );

    m_OriginalTrainTrackData [3][346].Init ( 8679, -13242, -40, 11282, 0 );

    m_OriginalTrainTrackData [3][347].Init ( 8561, -13106, -38, 11348, 98 );

    m_OriginalTrainTrackData [3][348].Init ( 8455, -12986, -37, 11410, 233 );

    m_OriginalTrainTrackData [3][349].Init ( 8342, -12856, -35, 11474, 64 );

    m_OriginalTrainTrackData [3][350].Init ( 8228, -12729, -34, 11538, 175 );

    m_OriginalTrainTrackData [3][351].Init ( 8133, -12624, -32, 11591, 152 );

    m_OriginalTrainTrackData [3][352].Init ( 7997, -12472, -31, 11667, 225 );

    m_OriginalTrainTrackData [3][353].Init ( 7879, -12342, -29, 11735, 132 );

    m_OriginalTrainTrackData [3][354].Init ( 7763, -12218, -27, 11797, 41 );

    m_OriginalTrainTrackData [3][355].Init ( 7664, -12112, -26, 11850, 25 );

    m_OriginalTrainTrackData [3][356].Init ( 7612, -12054, -26, 11882, 0 );

    m_OriginalTrainTrackData [3][357].Init ( 7526, -11965, -25, 11928, 5 );

    m_OriginalTrainTrackData [3][358].Init ( 7440, -11876, -24, 11973, 210 );

    m_OriginalTrainTrackData [3][359].Init ( 7366, -11797, -23, 12015, 115 );

    m_OriginalTrainTrackData [3][360].Init ( 7286, -11717, -22, 12058, 0 );

    m_OriginalTrainTrackData [3][361].Init ( 7217, -11648, -22, 12092, 46 );

    m_OriginalTrainTrackData [3][362].Init ( 7140, -11570, -21, 12134, 210 );

    m_OriginalTrainTrackData [3][363].Init ( 7041, -11470, -21, 12187, 64 );

    m_OriginalTrainTrackData [3][364].Init ( 6877, -11310, -20, 12274, 175 );

    m_OriginalTrainTrackData [3][365].Init ( 6792, -11228, -20, 12317, 155 );

    m_OriginalTrainTrackData [3][366].Init ( 6745, -11185, -20, 12340, 138 );

    m_OriginalTrainTrackData [3][367].Init ( 6670, -11116, -21, 12380, 160 );

    m_OriginalTrainTrackData [3][368].Init ( 6607, -11057, -21, 12412, 81 );

    m_OriginalTrainTrackData [3][369].Init ( 6542, -10995, -21, 12446, 197 );

    m_OriginalTrainTrackData [3][370].Init ( 6498, -10956, -21, 12468, 0 );

    m_OriginalTrainTrackData [3][371].Init ( 6414, -10880, -21, 12510, 36 );

    m_OriginalTrainTrackData [3][372].Init ( 6353, -10826, -20, 12540, 105 );

    m_OriginalTrainTrackData [3][373].Init ( 6292, -10770, -20, 12572, 86 );

    m_OriginalTrainTrackData [3][374].Init ( 6230, -10715, -20, 12604, 0 );

    m_OriginalTrainTrackData [3][375].Init ( 6192, -10681, -20, 12621, 154 );

    m_OriginalTrainTrackData [3][376].Init ( 6156, -10651, -19, 12640, 233 );

    m_OriginalTrainTrackData [3][377].Init ( 6072, -10579, -18, 12680, 64 );

    m_OriginalTrainTrackData [3][378].Init ( 6001, -10518, -16, 12716, 17 );

    m_OriginalTrainTrackData [3][379].Init ( 5937, -10463, -15, 12748, 202 );

    m_OriginalTrainTrackData [3][380].Init ( 5866, -10404, -14, 12782, 31 );

    m_OriginalTrainTrackData [3][381].Init ( 5820, -10364, -13, 12806, 160 );

    m_OriginalTrainTrackData [3][382].Init ( 5726, -10289, -10, 12851, 169 );

    m_OriginalTrainTrackData [3][383].Init ( 5665, -10241, -8, 12878, 56 );

    m_OriginalTrainTrackData [3][384].Init ( 5589, -10179, -5, 12917, 0 );

    m_OriginalTrainTrackData [3][385].Init ( 5524, -10128, -3, 12947, 36 );

    m_OriginalTrainTrackData [3][386].Init ( 5452, -10071, -1, 12983, 210 );

    m_OriginalTrainTrackData [3][387].Init ( 5369, -10008, 2, 13020, 115 );

    m_OriginalTrainTrackData [3][388].Init ( 5289, -9948, 5, 13058, 0 );

    m_OriginalTrainTrackData [3][389].Init ( 5171, -9861, 10, 13114, 72 );

    m_OriginalTrainTrackData [3][390].Init ( 5079, -9791, 14, 13159, 210 );

    m_OriginalTrainTrackData [3][391].Init ( 4947, -9700, 20, 13217, 117 );

    m_OriginalTrainTrackData [3][392].Init ( 4879, -9653, 24, 13250, 66 );

    m_OriginalTrainTrackData [3][393].Init ( 4791, -9593, 28, 13289, 116 );

    m_OriginalTrainTrackData [3][394].Init ( 4698, -9529, 32, 13329, 137 );

    m_OriginalTrainTrackData [3][395].Init ( 4645, -9495, 35, 13355, 11 );

    m_OriginalTrainTrackData [3][396].Init ( 4532, -9423, 41, 13405, 169 );

    m_OriginalTrainTrackData [3][397].Init ( 4412, -9350, 47, 13458, 185 );

    m_OriginalTrainTrackData [3][398].Init ( 4310, -9284, 53, 13504, 0 );

    m_OriginalTrainTrackData [3][399].Init ( 4178, -9208, 60, 13559, 36 );

    m_OriginalTrainTrackData [3][400].Init ( 4038, -9130, 67, 13620, 210 );

    m_OriginalTrainTrackData [3][401].Init ( 3915, -9060, 74, 13673, 115 );

    m_OriginalTrainTrackData [3][402].Init ( 3838, -9021, 78, 13706, 0 );

    m_OriginalTrainTrackData [3][403].Init ( 3711, -8958, 85, 13760, 0 );

    m_OriginalTrainTrackData [3][404].Init ( 3619, -8909, 90, 13798, 185 );

    m_OriginalTrainTrackData [3][405].Init ( 3511, -8856, 95, 13843, 186 );

    m_OriginalTrainTrackData [3][406].Init ( 3339, -8782, 104, 13913, 186 );

    m_OriginalTrainTrackData [3][407].Init ( 3208, -8725, 110, 13965, 0 );

    m_OriginalTrainTrackData [3][408].Init ( 3097, -8677, 116, 14011, 184 );

    m_OriginalTrainTrackData [3][409].Init ( 2780, -8561, 130, 14138, 186 );

    m_OriginalTrainTrackData [3][410].Init ( 2674, -8521, 134, 14180, 184 );

    m_OriginalTrainTrackData [3][411].Init ( 2607, -8502, 137, 14209, 0 );

    m_OriginalTrainTrackData [3][412].Init ( 2419, -8446, 143, 14281, 255 );

    m_OriginalTrainTrackData [3][413].Init ( 2240, -8392, 150, 14349, 187 );

    m_OriginalTrainTrackData [3][414].Init ( 2168, -8376, 152, 14377, 157 );

    m_OriginalTrainTrackData [3][415].Init ( 1985, -8335, 156, 14448, 0 );

    m_OriginalTrainTrackData [3][416].Init ( 1797, -8293, 161, 14522, 48 );

    m_OriginalTrainTrackData [3][417].Init ( 1674, -8274, 163, 14567, 159 );

    m_OriginalTrainTrackData [3][418].Init ( 1501, -8245, 166, 14634, 183 );

    m_OriginalTrainTrackData [3][419].Init ( 1341, -8221, 168, 14695, 0 );

    m_OriginalTrainTrackData [3][420].Init ( 1214, -8211, 168, 14743, 189 );

    m_OriginalTrainTrackData [3][421].Init ( 1026, -8194, 169, 14812, 156 );

    m_OriginalTrainTrackData [3][422].Init ( 876, -8182, 169, 14869, 144 );

    m_OriginalTrainTrackData [3][423].Init ( 784, -8181, 168, 14902, 0 );

    m_OriginalTrainTrackData [3][424].Init ( 636, -8179, 166, 14959, 133 );

    m_OriginalTrainTrackData [3][425].Init ( 459, -8177, 164, 15025, 110 );

    m_OriginalTrainTrackData [3][426].Init ( 370, -8176, 161, 15058, 107 );

    m_OriginalTrainTrackData [3][427].Init ( 209, -8176, 159, 15118, 0 );

    m_OriginalTrainTrackData [3][428].Init ( 21, -8176, 159, 15190, 105 );

    m_OriginalTrainTrackData [3][429].Init ( -166, -8176, 156, 15256, 108 );

    m_OriginalTrainTrackData [3][430].Init ( -355, -8176, 150, 15328, 112 );

    m_OriginalTrainTrackData [3][431].Init ( -542, -8176, 137, 15397, 0 );

    m_OriginalTrainTrackData [3][432].Init ( -637, -8176, 127, 15433, 107 );

    m_OriginalTrainTrackData [3][433].Init ( -729, -8176, 114, 15469, 110 );

    m_OriginalTrainTrackData [3][434].Init ( -850, -8181, 98, 15514, 111 );

    m_OriginalTrainTrackData [3][435].Init ( -973, -8195, 88, 15560, 0 );

    m_OriginalTrainTrackData [3][436].Init ( -1099, -8219, 82, 15609, 147 );

    m_OriginalTrainTrackData [3][437].Init ( -1226, -8250, 81, 15658, 121 );

    m_OriginalTrainTrackData [3][438].Init ( -1350, -8290, 84, 15706, 114 );

    m_OriginalTrainTrackData [3][439].Init ( -1471, -8337, 89, 15754, 0 );

    m_OriginalTrainTrackData [3][440].Init ( -1587, -8390, 98, 15803, 13 );

    m_OriginalTrainTrackData [3][441].Init ( -1695, -8450, 108, 15848, 143 );

    m_OriginalTrainTrackData [3][442].Init ( -1793, -8514, 120, 15894, 6 );

    m_OriginalTrainTrackData [3][443].Init ( -1881, -8582, 132, 15935, 0 );

    m_OriginalTrainTrackData [3][444].Init ( -1956, -8655, 146, 15973, 127 );

    m_OriginalTrainTrackData [3][445].Init ( -2016, -8730, 159, 16012, 127 );

    m_OriginalTrainTrackData [3][446].Init ( -2072, -8812, 172, 16048, 114 );

    m_OriginalTrainTrackData [3][447].Init ( -2133, -8900, 187, 16087, 0 );

    m_OriginalTrainTrackData [3][448].Init ( -2197, -8991, 202, 16128, 118 );

    m_OriginalTrainTrackData [3][449].Init ( -2265, -9086, 217, 16173, 109 );

    m_OriginalTrainTrackData [3][450].Init ( -2336, -9185, 233, 16220, 127 );

    m_OriginalTrainTrackData [3][451].Init ( -2406, -9280, 248, 16264, 0 );

    m_OriginalTrainTrackData [3][452].Init ( -2477, -9378, 263, 16309, 115 );

    m_OriginalTrainTrackData [3][453].Init ( -2547, -9470, 276, 16351, 117 );

    m_OriginalTrainTrackData [3][454].Init ( -2615, -9559, 289, 16392, 109 );

    m_OriginalTrainTrackData [3][455].Init ( -2680, -9642, 299, 16435, 0 );

    m_OriginalTrainTrackData [3][456].Init ( -2740, -9717, 308, 16469, 125 );

    m_OriginalTrainTrackData [3][457].Init ( -2795, -9785, 314, 16503, 127 );

    m_OriginalTrainTrackData [3][458].Init ( -2856, -9848, 319, 16537, 125 );

    m_OriginalTrainTrackData [3][459].Init ( -2932, -9906, 322, 16571, 0 );

    m_OriginalTrainTrackData [3][460].Init ( -3019, -9959, 325, 16609, 106 );

    m_OriginalTrainTrackData [3][461].Init ( -3116, -10005, 326, 16649, 110 );

    m_OriginalTrainTrackData [3][462].Init ( -3220, -10043, 328, 16691, 124 );

    m_OriginalTrainTrackData [3][463].Init ( -3330, -10070, 328, 16734, 0 );

    m_OriginalTrainTrackData [3][464].Init ( -3442, -10087, 329, 16776, 119 );

    m_OriginalTrainTrackData [3][465].Init ( -3556, -10092, 329, 16819, 106 );

    m_OriginalTrainTrackData [3][466].Init ( -3667, -10084, 328, 16861, 81 );

    m_OriginalTrainTrackData [3][467].Init ( -3774, -10058, 328, 16901, 0 );

    m_OriginalTrainTrackData [3][468].Init ( -3876, -10018, 328, 16942, 240 );

    m_OriginalTrainTrackData [3][469].Init ( -3969, -9958, 328, 16986, 155 );

    m_OriginalTrainTrackData [3][470].Init ( -4047, -9898, 328, 17020, 6 );

    m_OriginalTrainTrackData [3][471].Init ( -4111, -9848, 328, 17050, 0 );

    m_OriginalTrainTrackData [3][472].Init ( -4167, -9804, 328, 17078, 109 );

    m_OriginalTrainTrackData [3][473].Init ( -4218, -9766, 328, 17103, 87 );

    m_OriginalTrainTrackData [3][474].Init ( -4264, -9731, 328, 17125, 41 );

    m_OriginalTrainTrackData [3][475].Init ( -4311, -9695, 328, 17146, 0 );

    m_OriginalTrainTrackData [3][476].Init ( -4362, -9656, 328, 17170, 137 );

    m_OriginalTrainTrackData [3][477].Init ( -4418, -9611, 328, 17198, 131 );

    m_OriginalTrainTrackData [3][478].Init ( -4485, -9558, 328, 17230, 138 );

    m_OriginalTrainTrackData [3][479].Init ( -4563, -9492, 328, 17268, 0 );

    m_OriginalTrainTrackData [3][480].Init ( -4659, -9414, 328, 17315, 243 );

    m_OriginalTrainTrackData [3][481].Init ( -4773, -9319, 328, 17371, 173 );

    m_OriginalTrainTrackData [3][482].Init ( -4895, -9226, 331, 17426, 197 );

    m_OriginalTrainTrackData [3][483].Init ( -5015, -9148, 339, 17480, 0 );

    m_OriginalTrainTrackData [3][484].Init ( -5129, -9088, 351, 17530, 47 );

    m_OriginalTrainTrackData [3][485].Init ( -5241, -9043, 366, 17576, 46 );

    m_OriginalTrainTrackData [3][486].Init ( -5350, -9013, 384, 17617, 46 );

    m_OriginalTrainTrackData [3][487].Init ( -5454, -8999, 403, 17656, 0 );

    m_OriginalTrainTrackData [3][488].Init ( -5555, -8995, 422, 17695, 45 );

    m_OriginalTrainTrackData [3][489].Init ( -5652, -9006, 440, 17731, 40 );

    m_OriginalTrainTrackData [3][490].Init ( -5747, -9026, 457, 17768, 41 );

    m_OriginalTrainTrackData [3][491].Init ( -5838, -9057, 472, 17803, 0 );

    m_OriginalTrainTrackData [3][492].Init ( -5926, -9099, 482, 17840, 238 );

    m_OriginalTrainTrackData [3][493].Init ( -6010, -9149, 489, 17877, 203 );

    m_OriginalTrainTrackData [3][494].Init ( -6088, -9201, 492, 17914, 241 );

    m_OriginalTrainTrackData [3][495].Init ( -6158, -9253, 496, 17944, 0 );

    m_OriginalTrainTrackData [3][496].Init ( -6222, -9306, 501, 17976, 78 );

    m_OriginalTrainTrackData [3][497].Init ( -6280, -9362, 506, 18008, 79 );

    m_OriginalTrainTrackData [3][498].Init ( -6331, -9423, 513, 18035, 79 );

    m_OriginalTrainTrackData [3][499].Init ( -6377, -9491, 520, 18068, 0 );

    m_OriginalTrainTrackData [3][500].Init ( -6419, -9567, 529, 18099, 79 );

    m_OriginalTrainTrackData [3][501].Init ( -6455, -9656, 540, 18137, 80 );

    m_OriginalTrainTrackData [3][502].Init ( -6489, -9758, 553, 18176, 72 );

    m_OriginalTrainTrackData [3][503].Init ( -6521, -9875, 567, 18222, 0 );

    m_OriginalTrainTrackData [3][504].Init ( -6549, -10011, 583, 18274, 75 );

    m_OriginalTrainTrackData [3][505].Init ( -6574, -10168, 602, 18335, 77 );

    m_OriginalTrainTrackData [3][506].Init ( -6597, -10333, 622, 18395, 77 );

    m_OriginalTrainTrackData [3][507].Init ( -6619, -10495, 642, 18456, 0 );

    m_OriginalTrainTrackData [3][508].Init ( -6638, -10655, 661, 18516, 203 );

    m_OriginalTrainTrackData [3][509].Init ( -6660, -10810, 680, 18577, 198 );

    m_OriginalTrainTrackData [3][510].Init ( -6684, -10962, 697, 18635, 203 );

    m_OriginalTrainTrackData [3][511].Init ( -6712, -11105, 713, 18690, 0 );

    m_OriginalTrainTrackData [3][512].Init ( -6745, -11244, 728, 18742, 184 );

    m_OriginalTrainTrackData [3][513].Init ( -6784, -11374, 740, 18793, 182 );

    m_OriginalTrainTrackData [3][514].Init ( -6832, -11495, 750, 18841, 158 );

    m_OriginalTrainTrackData [3][515].Init ( -6889, -11605, 758, 18888, 0 );

    m_OriginalTrainTrackData [3][516].Init ( -6956, -11705, 762, 18934, 180 );

    m_OriginalTrainTrackData [3][517].Init ( -7036, -11792, 764, 18978, 34 );

    m_OriginalTrainTrackData [3][518].Init ( -7132, -11866, 761, 19023, 27 );

    m_OriginalTrainTrackData [3][519].Init ( -7242, -11920, 754, 19070, 0 );

    m_OriginalTrainTrackData [3][520].Init ( -7365, -11962, 741, 19118, 188 );

    m_OriginalTrainTrackData [3][521].Init ( -7498, -11991, 724, 19170, 19 );

    m_OriginalTrainTrackData [3][522].Init ( -7640, -12011, 703, 19224, 182 );

    m_OriginalTrainTrackData [3][523].Init ( -7791, -12025, 678, 19279, 0 );

    m_OriginalTrainTrackData [3][524].Init ( -7946, -12035, 651, 19339, 100 );

    m_OriginalTrainTrackData [3][525].Init ( -8105, -12044, 620, 19399, 101 );

    m_OriginalTrainTrackData [3][526].Init ( -8266, -12054, 586, 19459, 98 );

    m_OriginalTrainTrackData [3][527].Init ( -8428, -12067, 551, 19519, 0 );

    m_OriginalTrainTrackData [3][528].Init ( -8587, -12087, 513, 19580, 160 );

    m_OriginalTrainTrackData [3][529].Init ( -8744, -12116, 474, 19641, 180 );

    m_OriginalTrainTrackData [3][530].Init ( -8898, -12146, 436, 19699, 18 );

    m_OriginalTrainTrackData [3][531].Init ( -9055, -12165, 397, 19756, 0 );

    m_OriginalTrainTrackData [3][532].Init ( -9210, -12176, 360, 19817, 105 );

    m_OriginalTrainTrackData [3][533].Init ( -9366, -12176, 326, 19874, 101 );

    m_OriginalTrainTrackData [3][534].Init ( -9525, -12174, 293, 19934, 103 );

    m_OriginalTrainTrackData [3][535].Init ( -9685, -12166, 264, 19994, 0 );

    m_OriginalTrainTrackData [3][536].Init ( -9848, -12155, 238, 20057, 101 );

    m_OriginalTrainTrackData [3][537].Init ( -10018, -12144, 217, 20120, 102 );

    m_OriginalTrainTrackData [3][538].Init ( -10192, -12131, 199, 20186, 100 );

    m_OriginalTrainTrackData [3][539].Init ( -10371, -12119, 187, 20252, 0 );

    m_OriginalTrainTrackData [3][540].Init ( -10527, -12115, 181, 20309, 94 );

    m_OriginalTrainTrackData [3][541].Init ( -10630, -12113, 181, 20348, 94 );

    m_OriginalTrainTrackData [3][542].Init ( -10780, -12110, 181, 20405, 94 );

    m_OriginalTrainTrackData [3][543].Init ( -10952, -12108, 180, 20471, 0 );

    m_OriginalTrainTrackData [3][544].Init ( -11136, -12104, 176, 20540, 79 );

    m_OriginalTrainTrackData [3][545].Init ( -11340, -12101, 171, 20616, 79 );

    m_OriginalTrainTrackData [3][546].Init ( -11548, -12094, 164, 20694, 96 );

    m_OriginalTrainTrackData [3][547].Init ( -11759, -12085, 155, 20772, 0 );

    m_OriginalTrainTrackData [3][548].Init ( -11975, -12070, 147, 20853, 80 );

    m_OriginalTrainTrackData [3][549].Init ( -12189, -12051, 138, 20934, 96 );

    m_OriginalTrainTrackData [3][550].Init ( -12404, -12025, 129, 21016, 100 );

    m_OriginalTrainTrackData [3][551].Init ( -12616, -11990, 121, 21098, 0 );

    m_OriginalTrainTrackData [3][552].Init ( -12827, -11946, 114, 21177, 97 );

    m_OriginalTrainTrackData [3][553].Init ( -13032, -11893, 108, 21258, 95 );

    m_OriginalTrainTrackData [3][554].Init ( -13232, -11829, 104, 21337, 94 );

    m_OriginalTrainTrackData [3][555].Init ( -13422, -11754, 103, 21411, 0 );

    m_OriginalTrainTrackData [3][556].Init ( -13609, -11655, 103, 21493, 81 );

    m_OriginalTrainTrackData [3][557].Init ( -13792, -11534, 103, 21575, 121 );

    m_OriginalTrainTrackData [3][558].Init ( -13971, -11391, 103, 21661, 121 );

    m_OriginalTrainTrackData [3][559].Init ( -14144, -11228, 103, 21750, 0 );

    m_OriginalTrainTrackData [3][560].Init ( -14312, -11052, 103, 21841, 47 );

    m_OriginalTrainTrackData [3][561].Init ( -14472, -10867, 103, 21932, 30 );

    m_OriginalTrainTrackData [3][562].Init ( -14627, -10674, 103, 22024, 28 );

    m_OriginalTrainTrackData [3][563].Init ( -14775, -10474, 103, 22117, 0 );

    m_OriginalTrainTrackData [3][564].Init ( -14911, -10277, 103, 22207, 190 );

    m_OriginalTrainTrackData [3][565].Init ( -15039, -10082, 103, 22294, 86 );

    m_OriginalTrainTrackData [3][566].Init ( -15157, -9896, 103, 22376, 85 );

    m_OriginalTrainTrackData [3][567].Init ( -15262, -9718, 103, 22456, 0 );

    m_OriginalTrainTrackData [3][568].Init ( -15357, -9554, 103, 22526, 30 );

    m_OriginalTrainTrackData [3][569].Init ( -15441, -9400, 105, 22591, 30 );

    m_OriginalTrainTrackData [3][570].Init ( -15516, -9247, 109, 22657, 35 );

    m_OriginalTrainTrackData [3][571].Init ( -15582, -9097, 116, 22716, 0 );

    m_OriginalTrainTrackData [3][572].Init ( -15643, -8946, 124, 22778, 192 );

    m_OriginalTrainTrackData [3][573].Init ( -15694, -8792, 134, 22838, 84 );

    m_OriginalTrainTrackData [3][574].Init ( -15738, -8637, 145, 22901, 156 );

    m_OriginalTrainTrackData [3][575].Init ( -15775, -8477, 156, 22962, 0 );

    m_OriginalTrainTrackData [3][576].Init ( -15804, -8313, 166, 23023, 45 );

    m_OriginalTrainTrackData [3][577].Init ( -15828, -8144, 176, 23087, 191 );

    m_OriginalTrainTrackData [3][578].Init ( -15845, -7968, 184, 23153, 84 );

    m_OriginalTrainTrackData [3][579].Init ( -15857, -7782, 191, 23225, 0 );

    m_OriginalTrainTrackData [3][580].Init ( -15863, -7585, 196, 23297, 239 );

    m_OriginalTrainTrackData [3][581].Init ( -15861, -7382, 197, 23375, 113 );

    m_OriginalTrainTrackData [3][582].Init ( -15860, -7171, 197, 23453, 86 );

    m_OriginalTrainTrackData [3][583].Init ( -15860, -6966, 197, 23531, 0 );

    m_OriginalTrainTrackData [3][584].Init ( -15858, -6758, 197, 23609, 17 );

    m_OriginalTrainTrackData [3][585].Init ( -15858, -6552, 197, 23684, 21 );

    m_OriginalTrainTrackData [3][586].Init ( -15859, -6349, 197, 23762, 132 );

    m_OriginalTrainTrackData [3][587].Init ( -15860, -6144, 197, 23837, 0 );

    m_OriginalTrainTrackData [3][588].Init ( -15860, -5942, 197, 23915, 90 );

    m_OriginalTrainTrackData [3][589].Init ( -15862, -5739, 197, 23990, 96 );

    m_OriginalTrainTrackData [3][590].Init ( -15864, -5539, 197, 24065, 95 );

    m_OriginalTrainTrackData [3][591].Init ( -15866, -5339, 197, 24140, 0 );

    m_OriginalTrainTrackData [3][592].Init ( -15868, -5139, 197, 24215, 92 );

    m_OriginalTrainTrackData [3][593].Init ( -15869, -4936, 197, 24290, 20 );

    m_OriginalTrainTrackData [3][594].Init ( -15872, -4739, 197, 24365, 89 );

    m_OriginalTrainTrackData [3][595].Init ( -15872, -4640, 199, 24401, 0 );

    m_OriginalTrainTrackData [3][596].Init ( -15872, -4544, 197, 24437, 23 );

    m_OriginalTrainTrackData [3][597].Init ( -15873, -4416, 197, 24485, 23 );

    m_OriginalTrainTrackData [3][598].Init ( -15872, -4301, 197, 24530, 133 );

    m_OriginalTrainTrackData [3][599].Init ( -15871, -4160, 197, 24581, 0 );

    m_OriginalTrainTrackData [3][600].Init ( -15866, -4032, 197, 24629, 250 );

    m_OriginalTrainTrackData [3][601].Init ( -15859, -3904, 197, 24677, 133 );

    m_OriginalTrainTrackData [3][602].Init ( -15848, -3775, 197, 24728, 23 );

    m_OriginalTrainTrackData [3][603].Init ( -15835, -3649, 197, 24774, 0 );

    m_OriginalTrainTrackData [3][604].Init ( -15820, -3520, 197, 24822, 87 );

    m_OriginalTrainTrackData [3][605].Init ( -15806, -3392, 197, 24871, 87 );

    m_OriginalTrainTrackData [3][606].Init ( -15793, -3265, 197, 24919, 96 );

    m_OriginalTrainTrackData [3][607].Init ( -15780, -3136, 197, 24967, 0 );

    m_OriginalTrainTrackData [3][608].Init ( -15769, -3008, 197, 25015, 89 );

    m_OriginalTrainTrackData [3][609].Init ( -15761, -2880, 197, 25063, 87 );

    m_OriginalTrainTrackData [3][610].Init ( -15755, -2752, 197, 25111, 84 );

    m_OriginalTrainTrackData [3][611].Init ( -15744, -2624, 197, 25159, 0 );

    m_OriginalTrainTrackData [3][612].Init ( -15730, -2495, 197, 25211, 114 );

    m_OriginalTrainTrackData [3][613].Init ( -15718, -2410, 197, 25241, 109 );

    m_OriginalTrainTrackData [3][614].Init ( -15705, -2318, 197, 25278, 109 );

    m_OriginalTrainTrackData [3][615].Init ( -15690, -2220, 197, 25314, 0 );

    m_OriginalTrainTrackData [3][616].Init ( -15677, -2140, 197, 25345, 231 );

    m_OriginalTrainTrackData [3][617].Init ( -15663, -2053, 197, 25378, 135 );

    m_OriginalTrainTrackData [3][618].Init ( -15647, -1951, 197, 25418, 138 );

    m_OriginalTrainTrackData [3][619].Init ( -15639, -1883, 197, 25442, 0 );

    m_OriginalTrainTrackData [3][620].Init ( -15635, -1844, 197, 25457, 234 );

    m_OriginalTrainTrackData [3][621].Init ( -15622, -1728, 197, 25499, 134 );

    m_OriginalTrainTrackData [3][622].Init ( -15609, -1600, 197, 25547, 107 );

    m_OriginalTrainTrackData [3][623].Init ( -15603, -1522, 197, 25577, 0 );

    m_OriginalTrainTrackData [3][624].Init ( -15596, -1443, 197, 25608, 141 );

    m_OriginalTrainTrackData [3][625].Init ( -15590, -1343, 197, 25647, 7 );

    m_OriginalTrainTrackData [3][626].Init ( -15587, -1220, 197, 25692, 113 );

    m_OriginalTrainTrackData [3][627].Init ( -15587, -1064, 197, 25749, 0 );

    m_OriginalTrainTrackData [3][628].Init ( -15587, -867, 197, 25824, 150 );

    m_OriginalTrainTrackData [3][629].Init ( -15587, -670, 197, 25899, 53 );

    m_OriginalTrainTrackData [3][630].Init ( -15587, -459, 197, 25977, 60 );

    m_OriginalTrainTrackData [3][631].Init ( -15586, -298, 197, 26037, 0 );

    m_OriginalTrainTrackData [3][632].Init ( -15586, -161, 197, 26088, 58 );

    m_OriginalTrainTrackData [3][633].Init ( -15586, -32, 197, 26136, 56 );

    m_OriginalTrainTrackData [3][634].Init ( -15586, 87, 197, 26178, 213 );

    m_OriginalTrainTrackData [3][635].Init ( -15587, 164, 197, 26208, 0 );

    m_OriginalTrainTrackData [3][636].Init ( -15587, 199, 197, 26220, 27 );

    m_OriginalTrainTrackData [3][637].Init ( -15587, 229, 197, 26232, 227 );

    m_OriginalTrainTrackData [3][638].Init ( -15587, 289, 197, 26256, 132 );

    m_OriginalTrainTrackData [3][639].Init ( -15587, 309, 197, 26262, 0 );

    m_OriginalTrainTrackData [3][640].Init ( -15587, 327, 197, 26268, 29 );

    m_OriginalTrainTrackData [3][641].Init ( -15587, 371, 197, 26286, 60 );

    m_OriginalTrainTrackData [3][642].Init ( -15587, 430, 197, 26307, 53 );

    m_OriginalTrainTrackData [3][643].Init ( -15587, 482, 197, 26328, 0 );

    m_OriginalTrainTrackData [3][644].Init ( -15587, 540, 197, 26349, 55 );

    m_OriginalTrainTrackData [3][645].Init ( -15587, 590, 197, 26367, 57 );

    m_OriginalTrainTrackData [3][646].Init ( -15587, 631, 197, 26382, 215 );

    m_OriginalTrainTrackData [3][647].Init ( -15587, 679, 197, 26400, 0 );

    m_OriginalTrainTrackData [3][648].Init ( -15587, 736, 197, 26424, 9 );

    m_OriginalTrainTrackData [3][649].Init ( -15587, 799, 197, 26445, 216 );

    m_OriginalTrainTrackData [3][650].Init ( -15587, 857, 197, 26469, 29 );

    m_OriginalTrainTrackData [3][651].Init ( -15587, 909, 197, 26487, 0 );

    m_OriginalTrainTrackData [3][652].Init ( -15587, 958, 197, 26505, 8 );

    m_OriginalTrainTrackData [3][653].Init ( -15587, 1005, 197, 26523, 229 );

    m_OriginalTrainTrackData [3][654].Init ( -15586, 1024, 197, 26532, 216 );

    m_OriginalTrainTrackData [3][655].Init ( -15586, 1039, 197, 26535, 0 );

    m_bInitialised = false;
    Initialise ( );
}

CTrainTrackManager::~CTrainTrackManager ( )
{
    for ( int i = 0; i < NUM_RAILTRACKS; i++ )
    {
        // there are 4 train tracks so delete them all
        delete m_pTrainTracks[i];
        m_pTrainTracks[i] = NULL;
    }
}

CTrainTrack * CTrainTrackManager::GetTrainTrack ( unsigned char ucTrack )
{
    if ( ucTrack <= m_dwNumberOfTracks )
    {
        return m_pTrainTracks[ucTrack];
    }
    return nullptr;
}

void CTrainTrackManager::Initialise ( )
{
    if ( !m_bInitialised )
    {
        m_bInitialised = true;
        for ( int i = 0; i < MAX_TOTAL_TRACKS;i++ )
        {
            if ( i < NUM_RAILTRACKS )
            {
                m_pTrainTracks[i]->Initialise ( );
            }
            else
            {
                m_pTrainTracks[i] = NULL;
                UnreferenceTrainTrack ( i );
            }
        }

        m_dwNumberOfTracks = NUM_RAILTRACKS;

    }
}

bool CTrainTrackManager::GetOriginalRailNode ( unsigned char ucTrack, unsigned short usNode, SRailNode * pRailNode )
{
    if ( ucTrack < NUM_RAILTRACKS )
    {
        if ( m_OriginalTrainTrackData[ucTrack].find ( usNode ) != m_OriginalTrainTrackData[ucTrack].end ( ) )
        {
            SRailNode * pOriginalRailNode = &m_OriginalTrainTrackData[ucTrack][usNode];
            pRailNode->sX = pOriginalRailNode->sX;
            pRailNode->sY = pOriginalRailNode->sY;
            pRailNode->sZ = pOriginalRailNode->sZ;
            pRailNode->fRailDistance = pOriginalRailNode->fRailDistance;
            return true;
        }
    }
    return false;
}

void CTrainTrackManager::ResetTracks ( )
{
    for ( int i = 0; i < NUM_RAILTRACKS; i++ )
    {
        // there are 4 train tracks so reset them all
        m_pTrainTracks[i]->Reset ( );
    }

    m_dwNumberOfTracks = MAX_TOTAL_TRACKS;
}

CTrainTrack * CTrainTrackManager::CreateTrainTrack ( const std::vector<CVector>& vecNodeList, CElement * pParent, CXMLNode * pNode )
{
    if ( m_dwNumberOfTracks < MAX_TOTAL_TRACKS )
    {
        CTrainTrack * pTrainTrack = new CTrainTrack ( m_dwNumberOfTracks, this, pParent, pNode );
        m_pTrainTracks[m_dwNumberOfTracks] = pTrainTrack;

        // Reallocate and initialise
        Reallocate ( pTrainTrack, vecNodeList.size() );

        pTrainTrack->Initialise();
        
        // Set our number of tracks and update a comparison instruction
        m_dwNumberOfTracks++;

        auto it = vecNodeList.begin ();
        uint uiNode = 0;
        for ( it; it != vecNodeList.end (); it++ ) {
            pTrainTrack->SetRailNodePosition ( uiNode, *it );
            uiNode++;
        }

        return pTrainTrack;
    }
    return NULL;
}

bool CTrainTrackManager::UnreferenceTrainTrack ( DWORD dwTrackID )
{
    if ( dwTrackID < MAX_TOTAL_TRACKS )
    {
        // Set our data to default
        m_fRailTrackLengths[dwTrackID]      = 0;
        m_dwNumberOfTrackNodes[dwTrackID]   = 1;

        // Unreference our train track
        m_pTrainTracks[dwTrackID] = NULL;

        return true;
    }
    return false;
}


bool CTrainTrackManager::Reallocate ( CTrainTrack * pTrainTrack, unsigned int uiNodes )
{
    // Get our track ID
    DWORD dwTrackID = pTrainTrack->GetTrackID ( );

    // Set our data
    m_dwNumberOfTrackNodes[dwTrackID]   = uiNodes;

    CVector vecDefaultPositions[2];
    vecDefaultPositions[0] = CVector( 2811.3762207031f, 1645.107421875f, 9.8203125f + 2.4973087310791f );
    vecDefaultPositions[1] = CVector( 2811.3762207031f, 1657.9461669922f, 9.8203125f + 2.4973087310791f  );
    CVector vecDistance = (vecDefaultPositions[0] - vecDefaultPositions[1]) / (float)uiNodes;
    for ( unsigned int i = 0; i < uiNodes; i++ )
        m_pTrainTracks[dwTrackID]->SetRailNodePosition ( i, vecDefaultPositions[0] + (vecDistance * (float)i) );

    // Initialise our track
    m_pTrainTracks[dwTrackID]->Initialise ( );

    m_fRailTrackLengths[dwTrackID]      = m_pTrainTracks[dwTrackID]->GetTrackLength ( );
    return true;
}
