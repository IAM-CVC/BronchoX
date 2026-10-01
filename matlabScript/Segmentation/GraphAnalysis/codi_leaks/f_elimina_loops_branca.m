
function [nodes_elim, TG] = f_elimina_loops_branca(TG)

nodes_elim = [];
fi = false;

while (fi==false) 
   
    %trobem els loops
   
    LoopNodes = [];
     if (TG.complexity>0)
        [ GSubLeak ] = GraphLoops( TG ); 

        for kL=1:length(GSubLeak)
            LoopNodes=[LoopNodes GSubLeak{kL}.LoopNodes];
        end
     end

    %si no hi ha loops hem acabat, sino continuem
    if (isempty(LoopNodes))
        fi = true;
        disp('no hi ha més nodes a eliminar, no hi ha loops');
    
    %si encara hi ha loops
    else
        
        %elimina el node de minima energia i el desem a la llista   
        [node_min_ener, TG, is_root] = f_elimina_node_minima_energia(TG, LoopNodes);
        nodes_elim = unique([nodes_elim; node_min_ener]);

        %si hem eliminat alguna cosa analitzem el components
        nodes_desconnectats = [];
        if (~isempty(node_min_ener))
             if (is_root==false)   
                %detecta nodes desconnectats i els elimina
                [nodes_desconnectats, TG] = f_elimina_nodes_desconnectats(TG);

             else 
                %si era el root ens carreguem tot l'arbre
                nodes_desconnectats = [1:size(TG.e,1)]';
                TG.e(:) = 0;
                 fi = true;
                disp('no hi ha més nodes a eliminar, hem suprimit el root');
             end   
            %actualitzar nodes eliminats
            nodes_elim = unique([nodes_elim; nodes_desconnectats]);
        end

        %recalculem el graf 
        if (fi==false)   
            TG = ComplexityPropsDeb(TG,'Leaf',TG.rootNode);
        end
    end
    
    %veure el graf
    % view(biograph(Graph2dirGraph(TG.e,TG.rootNode))); 

end
