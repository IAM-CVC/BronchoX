%% 0. ENVIROMENT SETTINGS
clear all
Machine='PC';
CPAP_ExpSettings;
addpath('D:\Funcions Matlab\CNNs\deepmat-master')

% CTStudies=dir([IAMDataDir filesep StudyID '*']);
% CTStudies={CTStudies.name};
% NStudy=length(CTStudies);
% save([DataExp filesep 'CTStudies'],'CTStudies');
load([DataExp filesep 'CTStudies']);

% LENS10 is discarded cause it only has 1 lung and CTMainAirwaysSeg fails
% in this case
CTStudies={CTStudies{2:end}};

NStudy=length(CTStudies);
NCTCase=length(CTCaseIDs);
NCPAP=length(CPAPIDs);

ExpFolder='25_01_2017';
IAMFolder='Z:\Experiments\CTBronquiSeg\CPAP';

NoVessel=1;
OutFileID='RegalCumple';
OutFileID=[OutFileID '_NoVessel_' num2str(NoVessel)];

SegIdx=2;

%% 1. EXPORT 2 OBJ
for kStudy=1:NStudy
    CTStudy=CTStudies{kStudy};
    
    for kCase=1:NCTCase
        CTCase=CTCaseIDs{kCase};
        CaseOutputDir=[DataExp filesep CTStudy filesep CTCase filesep ExpFolder];
        cd(CaseOutputDir);
        load(['BronchiSeg_'  OutFileID ]);
        for SegIdx=1:2
            BronchiDist=bwdist(BronchiSeg{SegIdx});
            CTSeg2Obj(BronchiDist,1.5,[DataExp filesep CTStudy filesep CTStudy '_' CTCase '_Seg' num2str(SegIdx) '.obj'],pixsize);
        end
    end
end

%% 2. COPY FILES TO SHARED
for kStudy=1:NStudy
    CTStudy=CTStudies{kStudy};
    
    for kCase=1:NCTCase
        CTCase=CTCaseIDs{kCase};
        CaseOutputDir=[DataExp filesep CTStudy filesep CTCase filesep ExpFolder];
        IAMOutputDir=[IAMFolder filesep CTStudy filesep CTCase];
        mkdir(IAMOutputDir);

        copyfile([CaseOutputDir filesep 'CTData.mat'],[IAMOutputDir filesep 'CTData.mat']);
        copyfile([CaseOutputDir filesep 'CTDataROI.mat'],[IAMOutputDir filesep 'CTDataROI.mat']);
        for SegIdx=1:2
            copyfile([DataExp filesep CTStudy filesep CTStudy '_' CTCase '_Seg' num2str(SegIdx) '.obj'],[IAMFolder filesep CTStudy filesep CTStudy '_' CTCase '_Seg' num2str(SegIdx) '.obj'])
        end
    end
    
end

for kStudy=1:NStudy
    CTStudy=CTStudies{kStudy};
    
    for kCase=1:NCTCase
        CTCase=CTCaseIDs{kCase};
        CaseOutputDir=[DataExp filesep CTStudy filesep CTCase filesep ExpFolder];
        IAMOutputDir=[IAMFolder filesep CTStudy filesep CTCase filesep ExpFolder];
        mkdir(IAMOutputDir);
        
        copyfile([CaseOutputDir filesep 'BronchiEner_' OutFileID '.mat'],[IAMOutputDir filesep 'BronchiEner.mat']);
        copyfile([CaseOutputDir filesep 'BronchiSeg_' OutFileID '.mat'],[IAMOutputDir filesep 'BronchiSeg.mat']);

    end
    
end