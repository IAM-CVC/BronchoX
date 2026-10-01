%% 2.2 DISTAL GLOBAL SEGMENTATION WITH THRESHOLD ADAPTATION
function [AirwaySeg,SegParam]=AirwaySeg_with_GlobalThAdaptation_Iterative( AirwaySeg,AirwayEner,AirwayWallEner,SegParam)


ThAll=SegParam.ThAll;
NTh=length(ThAll);

for k=NTh:-1:1
    SegParam.Th=ThAll(k);
    [ CurrentSeg] = AirwaysSeg_from_Eners(AirwaySeg,AirwayEner,AirwayWallEner,SegParam );
    
    AreaIncr=sum(CurrentSeg(:)-AirwaySeg(:))/sum(AirwaySeg(:));
    if(AreaIncr>.25)
        return;
    else
        [ GNew,GSubNew] = SegComplexity_23_10_2018( CurrentSeg);
        if (GNew.complexity>SegParam.GlobalComplexityTh)
            return;
        else
            AirwaySeg=CurrentSeg;
        end
        
    end
    

end


