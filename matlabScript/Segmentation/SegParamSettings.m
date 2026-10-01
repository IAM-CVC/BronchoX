
%% 1. PARAMETER SETTINGS

% Lungs/Trachea Segmentation

LungSegParam.d_seROI=10;
%LugSegParam.d_seROI=10; % For .5 slices
%LugSegParam.d_seROI=5; % For 1.0 slices
LungSegParam.d_seTrachea=5;
LungSegParam.BodyHU=-100;
LungSegParam.LungHU=[-600,-950];
SepDistParam.d=[5]; 
SepDistParam.d_se=5;
LungSegParam.SepDistParam=SepDistParam;

N=1;
hTh=pi/N;
TracheaEnerParams.sig=[1];
TracheaEnerParams.scale=1;
TracheaEnerParams.Th1=[[0:1:N-1]*hTh];
TracheaEnerParams.Th2=[[0:1:N-1]*hTh]';
TracheaEnerParams.offset=[2,1,1];
TracheaEnerParams.FiltType='DNR_Bola_GPU';
TracheaEnerParams.ConvType='Fourier';

% Main Bronchi Segmentation
N=6;
hTh=pi/N;
MainBronchiEnerParams.FParam.scale=1;
MainBronchiEnerParams.FParam.sig=[1];
MainBronchiEnerParams.FParam.Th1=[[0:1:N-1]*hTh];
MainBronchiEnerParams.FParam.Th2=[[0:1:N-1]*hTh]';
MainBronchiEnerParams.FParam.offset=[4,1,1];
MainBronchiEnerParams.FParam.ConvType='Fourier';
MainBronchiEnerParams.FParam.FiltType='DNR_Gauss_GPU';
%% AQUESTS PARAMETRES DE DOWNSAMPLING DEPENEN DE LA RESOLUCIÓ. CALDRIA VEURE SI EL REGION GROWING DE SEMPRE NO ES IGUAL D'EFECTIU
MainBronchiEnerParams.RedParams.ratio=2;
MainBronchiEnerParams.RedParams.NLevels=2;


MainBronchiSegParams.ThWall=1;
MainBronchiSegParams.ThBronchiDist=2;
MainBronchiSegParams.ThWallDist=1;
MainBronchiSegParams.ThType='MainAirPercentile';
MainBronchiSegParams.Prc=80;
MainBronchiSegParams.d_se=4;
MainBronchiSegParams.Conn=6;

% Main Airways Segmentation (Els d'energia son identics als dels bronquis)
N=6;
hTh=pi/N;
MainAirwaysEnerParams.scale=1;
MainAirwaysEnerParams.sig=[1];
MainAirwaysEnerParams.Th1=[[0:1:N-1]*hTh];
MainAirwaysEnerParams.Th2=[[0:1:N-1]*hTh]';
MainAirwaysEnerParams.offset=[4,1,1];
MainAirwaysEnerParams.ConvType='Fourier';
MainAirwaysEnerParams.FiltType='DNR_Gauss_GPU';

MainAirwaysSegParam.ThWall=.1;
MainAirwaysSegParam.ThBronchiDist=4;
MainAirwaysSegParam.ThWallDist=2;
MainAirwaysSegParam.ThType='MainAirPercentile';
MainAirwaysSegParam.Prc=[80];
MainAirwaysSegParam.Conn=6;

% MainAirwaysSegParam.ThType='Fixed';
% MainAirwaysSegParam.ThBronchiEner=.2;

% Distal Bronchi Segmentation (Els d'energia son identics als dels bronquis principals)
N=6;
hTh=pi/N;
DistalEnerParams.scale=1;
DistalEnerParams.sig=[1];
DistalEnerParams.Th1=[[0:1:N-1]*hTh];
DistalEnerParams.Th2=[[0:1:N-1]*hTh]';
DistalEnerParams.offset=[4,1,1];
DistalEnerParams.ConvType='Fourier';
DistalEnerParams.FiltType='DNR_Gauss_GPU';

DistalSegParam.ThWall=700;
DistalSegParam.ThBronchiDist=4;
DistalSegParam.ThWallDist=1;
DistalSegParam.Conn=26;
