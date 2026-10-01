%% 2.2 DISTAL GLOBAL SEGMENTATION WITH THRESHOLD ADAPTATION
function [AirwaySeg,G,GSub,SegParam]=AirwaySeg_with_GlobalThAdaptation( AirwaySeg0,AirwayEner,AirwayWallEner,SegParam)

CITh=SegParam.CITh;
ThAct=SegParam.Th;
CITh_Diff=SegParam.CITh_Diff;

% Complexity of initial segmentation
[ GNew,GSubNew ] =SegComplexity_06_11_2017( AirwaySeg0 );
AirwaySeg=AirwaySeg0;
G=GNew;
GSub=GSubNew;


while((diff(CITh)>CITh_Diff))
    
    [ CurrentSeg] = AirwaysSeg_from_Eners(AirwaySeg0,AirwayEner,AirwayWallEner,SegParam );
    
    AreaIncr=sum(CurrentSeg(:)-AirwaySeg(:))/sum(AirwaySeg(:));
    if(AreaIncr>.25)
        GlobalComplexity=1;
    else
        [ GNew,GSubNew] = SegComplexity_23_10_2018( CurrentSeg);
        GlobalComplexity =GNew.complexity;
        
    end
    
    if(GlobalComplexity<=SegParam.GlobalComplexityTh)
        AirwaySeg=CurrentSeg;
        G=GNew;
        GSub=GSubNew;
        SegParam.Th=ThAct;
    end
    
    [CITh,ThAct]=Update_Th(GlobalComplexity,CITh,ThAct,SegParam.GlobalComplexityTh);
    SegParam.Th=ThAct;
    
end


