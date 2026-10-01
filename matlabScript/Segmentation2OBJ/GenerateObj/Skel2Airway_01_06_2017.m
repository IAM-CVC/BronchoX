%==========================================================================
% FUNCTION NAME : [volReconFIN,indRecon_RProjFIN,linkRecon]=Skel2Airway(link,voldist,ReconType,skelVol);
%--------------------------------------------------------------------------
% DESCRIPTION : Airway Volume Reconstruction from links codifying vol
% skeleton. Reconstruction is given by inverse skeletonization using skel dist map
% given by volume voldist. Two reconstructions are supported: 'Linear', 'Original'.
%
%
%--------------------------------------------------------------------------
% INPUTS :
% 1> link:  Structure array containing skelvol branches computed using any Skel2Graph3D function:
%       link.point: list of volume indexs corresponding to points in the
%       branch using ind2sub
% 2> voldist: skel dist map for computation of inverse skeletonization
% 3> ReconType: 'Original' for reconstrion using link.point
%               'Linear' for linear approximation to links
% 4> skelVol: skel volume mask to obtain a single connected component. This
% should be ommited in the future the moment link.point provides a single connected
% component
%--------------------------------------------------------------------------
% OUTPUTS :  
% 1> volReconFIN: Reconstructed volume mask
% 2> indRecon_RProjFIN: List of indexes of reconstructed volReconFIN voxels 
% 3> linkRecon: branches used to perform inverse skeletonization ('Linear'
% case)
%--------------------------------------------------------------------------
% EXTERNAL FUNCTIONS :
% Vol2TubularIdx_Agnes,TubularIdx2VolRecon,CTSeg2Obj
%--------------------------------------------------------------------------
% RELATED BIBLIOGRAPHY :
%--------------------------------------------------------------------------
% CREATION DATE : 09 / 03 / 2016
%--------------------------------------------------------------------------
% LAST MODIFICATION :
%--------------------------------------------------------------------------
% AUTHOR : Debora Gil
%--------------------------------------------------------------------------
% USAGE EXAMPLES :
% load('skel_LENS10_INSPI_SIN.mat') %%% Esquelet Carles sense loops ni endpoints
% volimage=PrincipalConnComp(volimage,26,1);
% w=size(volimage,1);
% l=size(volimage,2);
% h=size(volimage,3);
% voldist=bwdist(1-volimage);
% [volReconFIN,indRecon_RProjFIN,linkRecon]=Skel2Airway(link,voldist,ReconType,skelVol);
%==========================================================================

function [volReconFIN,indRecon_RProjFIN,linkRecon]=Skel2Airway_01_06_2017(link,voldist,ReconType)

w=size(voldist,1);
l=size(voldist,2);
h=size(voldist,3);

switch ReconType
    case 'Original'
        linkRecon=link;
        %%% Agnes Eficient implementation (snif,snif)
        [indRecon_RProjFIN]=Vol2TubularIdx_Agnes( linkRecon,voldist );
        %%% Volume Reconstruction
        ReconParam.ThRecon=2;
        ReconParam.imsize=size(voldist);
        [ volReconFIN ] = TubularIdx2VolRecon(indRecon_RProjFIN, ReconParam );
    case 'Linear'
        % Linear Branches
        t=0:.05:1;
        for k=1:length(link)
            
            [yP,xP,zP]=ind2sub([w,l,h],(link(k).point));
            xP=xP(1).*t+(1-t).*xP(end);
            yP=yP(1).*t+(1-t).*yP(end);
            zP=zP(1).*t+(1-t).*zP(end);
            linkRecon(k).point=sub2ind([w,l,h],round(yP),round(xP),round(zP));
            linkRecon(k).x=xP;
            linkRecon(k).y=yP;
            linkRecon(k).z=zP;
        end
        [indRecon_RProjFIN]=Vol2TubularIdx_Agnes( linkRecon,voldist );
        %%% Volume Reconstruction
        ReconParam.ThRecon=2;
        ReconParam.imsize=size(voldist);
        [ volReconFIN ] = TubularIdx2VolRecon(indRecon_RProjFIN, ReconParam );
        
        
end

%%%% Fill Gaps due to link structure artifacts that leave unconnected
%%%% branches
%% OBS: This is to be removed in the future
% indAll=find(skelVol);
% indDiff=setdiff(indAll,[link(:).point]);
% skelPrunEEDif=0*volReconFIN;
% skelPrunEEDif(indDiff)=1;
% volReconFIN=max(bwdist(skelPrunEEDif)<=1,volReconFIN);
end




