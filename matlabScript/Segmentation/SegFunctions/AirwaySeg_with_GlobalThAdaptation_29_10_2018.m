%% 2.2 DISTAL GLOBAL SEGMENTATION WITH THRESHOLD ADAPTATION
function [AirwaySeg,G,SegParam]=AirwaySeg_with_GlobalThAdaptation_29_10_2018( AirwaySeg0,AirwayEner,AirwayWallEner,SegParam)

CITh=SegParam.CITh;
ThAct=SegParam.Th;
CITh_Diff=SegParam.CITh_Diff;

% Complexity of initial segmentation
[ GNew,GSubNew ] =SegComplexity_06_11_2017( AirwaySeg0 );
AirwaySeg=AirwaySeg0;
G=GNew;
GSub=GSubNew;

it=1;
while((diff(CITh)>CITh_Diff))
    
    % Funcion que segmenta
    [ CurrentSeg] = AirwaysSeg_from_Eners(AirwaySeg,AirwayEner,AirwayWallEner,SegParam );
    
    AreaIncr=sum(CurrentSeg(:)-AirwaySeg(:))/sum(AirwaySeg(:));
    if(AreaIncr>.25)
        GlobalComplexity=1;
    else
        skel=Skeleton3D(CurrentSeg);
        [ GNew] = SegBranchLocalComplexity_26_10_2018( skel);
        GlobalComplexity =GNew.complexity;
        
    end
    
    if(GlobalComplexity<=SegParam.GlobalComplexityTh)
        AirwaySeg=CurrentSeg;
        G=GNew;
        SegParam.Th=ThAct;
    end
    
    [CITh,ThAct]=Update_Th(GlobalComplexity,CITh,ThAct,SegParam.GlobalComplexityTh);
    SegParam.Th=ThAct;
    it=it+1
end



