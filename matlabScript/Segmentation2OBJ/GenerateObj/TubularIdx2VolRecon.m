function [ volRecon ] = TubularIdx2VolRecon(indRecon_RProj, ReconParam )

%%% Input Parameters
ThRecon=ReconParam.ThRecon;
imsize=ReconParam.imsize;

volRecon=zeros(imsize);
volRecon(indRecon_RProj)=1;
%volRecon=PrincipalConnComp(volRecon,26,1);

if(ThRecon>0)
    volRecon_dist=bwdist(volRecon);
    volRecon_dist=bwdist(1-(volRecon_dist<=ThRecon));
    volRecon=volRecon_dist>=ThRecon;
end
%volRecon=PrincipalConnComp(volRecon,26,1);

end

