
function [] = f_show_graph(TG,LoopNodes)

 if(prod(size(TG.e))>1)
        TG.e = Graph2dirGraph(TG.e,TG.rootNode);
        bg = biograph(TG.e);
        nnodes = size(TG.v,1);
        
        v=[];
        for i=1:nnodes      
            v(i) = TG.wv(i,2);
            set(bg.nodes(i),'Color',[v(i) v(i) v(i)]);
        end
  
         
        for kL=1:length(LoopNodes)
            set(bg.nodes(LoopNodes(kL)),'LineColor',[1 0 0]);           
        end
        
        h = view(bg);
    end