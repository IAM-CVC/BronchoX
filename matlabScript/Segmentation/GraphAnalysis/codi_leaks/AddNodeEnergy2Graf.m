function [GSub]=AddNodeEnergy2Graf(BronchiEner,MainLocalTh,GSub)

BronchiEnerNorm = BronchiEner/max(BronchiEner(:));


for k=1:numel(GSub)
    TG=GSub{k};
    if(~isempty(TG.e))
        nnodes = size(TG.v,1);
        v=[];
        for i=1:nnodes
            p = round([TG.v(i,1),TG.v(i,2),TG.v(i,3)]);
            v(i) = max(BronchiEnerNorm(p(1),p(2),p(3)),MainLocalTh(p(1),p(2),p(3)));
            GSub{k}.wv(i,2)=v(i);
        end
      %  GSub{k}.wv=[GSub{k}.wv,v(:)];
       
    end
end


