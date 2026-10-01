%==========================================================================
% FUNCTION NAME : [LapR]=TubeDetector3D_GPU(FParam,im);
%--------------------------------------------------------------------------
% DESCRIPTION : Blob detector for 3D volumes by convolution of the volume im
% with a bank of oriented Laplacian
%
%
%--------------------------------------------------------------------------
% INPUTS :
% 1> FParam:  Structure containing Laplacian parameters:
%       FParam.Th: Norientations x 2 vector indicating the 2 orientations
%              of the 3D anisotropic gaussian used to compute Laplacians using the
%              function Anisotropic_dGauss3D
%       FParam.sig: escale of the gaussian filter used to compute
%              Laplacians (see Anisotropic_dGauss3D)
%       FParam.offset: 3D vector specifying gaussian anisotropy (see Anisotropic_dGauss3D)
%       FParam.FiltType: Laplacian filter type:
%              'Normal', for standard Gxx+Gyy+Gzz
%              'Lind', for Lindenberg normalization Gxx/sigx+Gyy/sigy+Gzz/sigz
%              'DNR', for L2 normallization of the Laplacian filter
%              'NIF', for double L2 normalization (filter and vol local
%              norm, see DescriptorsNormResponse3D)
% 2> im: Input Volume
%--------------------------------------------------------------------------
% OUTPUTS :  LapR: Structure containing the maximum response to the bank of Laplacian
% filters:
%      LapR.MxDNRimN: Negative responses (ligth blobs)
%      LapR.MxScN: Scale of the filter achieving max negative responses (ligth blobs)
%      LapR.MxDNRimP: Positive responses (dark blobs)
%      LapR.MxScP: Scale of the filter achieving max positive responses (dark blobs)
%--------------------------------------------------------------------------
% EXTERNAL FUNCTIONS :
% StandardLaplacian3D,LindebergLaplacian3D,NormalizedLaplacian3D,
% DescriptorsResponse3D
%--------------------------------------------------------------------------
% RELATED BIBLIOGRAPHY :
%--------------------------------------------------------------------------
% CREATION DATE : 03 / 02 / 2015
%--------------------------------------------------------------------------
% LAST MODIFICATION :
%--------------------------------------------------------------------------
% AUTHOR : Debora Gil
%--------------------------------------------------------------------------
% USAGE EXAMPLES :
%
% [LapR]=TubeDetector3D_GPU(FParam,im);
%==========================================================================
function [LapR]=TubeDetector3D_GPU(FParam,im)

%% Filter Bank
Th1=FParam.Th1;
Th2=FParam.Th2;
sig=FParam.sig;
offset=FParam.offset;
scale=FParam.scale;
FiltType=FParam.FiltType;
ConvType=FParam.ConvType;


switch FiltType
    
        
    case 'DNR_Gauss_GPU'
        
        %%%%%%%%%%%%% Filter Bank
        [Filt]=Tubular_dGauss3D_GPU(Th1,Th2,sig,offset);
       
    case 'DNR_Bola_GPU'
        
        %%%%%%%%%%%%% Filter Bank
        [Filt]=Tubular_Bola3D_GPU(Th1,Th2,sig,offset);
        
        
        
end

%%%%%%%%%%%%% Filter Response 
[LapR]=DescriptorsResponse3D_GPU([im],Filt,ConvType);
        

