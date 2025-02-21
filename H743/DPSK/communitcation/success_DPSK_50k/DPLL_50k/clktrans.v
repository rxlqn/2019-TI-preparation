module clktrans(
	rst_n_i	,
	clk32_i	,
	clk_d1_o,
	clk_d2_o		
);
	input		wire			rst_n_i	;
	input		wire			clk32_i	;
	output	reg				clk_d1_o;
	output	reg				clk_d2_o;
	
	reg	[1:0]	cnt_clk32	; wire	add_cnt_clk32 , end_add_cnt_clk32 ;
	
	always @(posedge clk32_i or negedge rst_n_i)
	begin
		if(!rst_n_i)
			cnt_clk32	<= 2'd0 ;
		else 
			if(add_cnt_clk32)
		begin
			if(end_add_cnt_clk32)
				cnt_clk32 <= 2'd0 ;
			else
				cnt_clk32	<= cnt_clk32 + 1'b1 ;
			end
		else
			cnt_clk32 	<= 2'd0	;
	end	
	
	assign	add_cnt_clk32 = rst_n_i	;
	assign	end_add_cnt_clk32 = add_cnt_clk32 && cnt_clk32 == 4 - 1 ;
	
	always @(posedge clk32_i or negedge rst_n_i)
	begin
		if(!rst_n_i)
			clk_d1_o	<= 1'b0 ;
		else 
		if(add_cnt_clk32)
			begin
				if(cnt_clk32 == 2'd0)
					clk_d1_o 	<= 1'b1 ;
				else
					clk_d1_o	<= 1'b0 ;
			end
		else
			clk_d1_o 		<= 1'b0	;
	end
	
	always @(posedge clk32_i or negedge rst_n_i)
	begin
		if(!rst_n_i)
			clk_d2_o	<= 1'b0 ;
		else 
		if(add_cnt_clk32)
			begin
				if(cnt_clk32 == 2'd2)
					clk_d2_o 	<= 1'b1 ;
				else
					clk_d2_o	<= 1'b0	;
			end
		else
			clk_d2_o 		<= 1'b0	;
	end
	
endmodule
