	$(".node").each(function() {
		placeNode($(this));
	});
	
	$(".space").each(function() {
		placeSpace($(this));
	});
	
	$(".data").each(function() {
		placeData($(this));
	});	


	
	function placeSpace(space){
		var w = space.data('x2')-space.data('x1');
		var h = space.data('y2')-space.data('y1');

		var x = space.data('x1');
		var y = 1000-h-space.data('y1');
		
		space.width(w).height(h);
		space.css('background-color', space.data('color'));
		space.css({left:x,top:y});	
	}
	
	function placeNode(node){		
		var x = node.data('x')-4;
		var y = 1000-node.data('y')-4;
		
		node.width(8).height(8);
		node.css('background-color', 'black');
		node.css({left:x,top:y});	
	}
	
	function placeData(d){		
		var x = d.data('x')-2;
		var y = 1000-d.data('y')-2;
		
		d.width(5).height(5);
		d.css('background-color', 'red');
		d.css({left:x,top:y});	
	}	
	
	
	
	