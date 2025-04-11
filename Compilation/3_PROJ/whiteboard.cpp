| TYPE id_token '(' FUNC_ARGLIST ')' 
		{
			if(functionTable.find($2.name) == functionTable.end()){
				for (size_t i = 0; i < $4.paramIds.size(); ++i){
        			for (size_t j = i + 1; j < $4.paramIds.size(); ++j){
            			if ($4.paramIds[i] == $4.paramIds[j]){
							cerr << "Semantic error: '" << $2.name << "' function has two veriebles with the same name  ";
							cerr << "in line number " << to_string(yylineno) << endl;
							exit(0);
						}	
					}
				}
				Function f;
				f.isDefined = 0;
				f.startIndexOptionalParams = 0;
				f.returnType = $1.type;
				f.paramTypes = $4.paramTypes;
				f.paramIds = $4.paramIds;
				f.startLineImplementation = buffer.nextQuad();
				functionTable[$2.name] = f;
			}
			else{
				if(functionTable[$2.name].isDefined == 1){
					cerr << "Semantic error: '" << $2.name << "' function has multiple definitions ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				if(functionTable[$2.name].startIndexOptionalParams != 0){
					cerr << "Semantic error: '" << $2.name << "' function has conflicting declaration ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				if (functionTable[$2.name].paramTypes != $4.paramTypes) {
					cerr << "Semantic error: '" << $2.name << "' function has conflicting declaration ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				if (functionTable[$2.name].paramIds != $4.paramIds) {
					cerr << "Semantic error: '" << $2.name << "' function has conflicting declaration ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				if (functionTable[$2.name].returnType != $1.type) {
					cerr << "Semantic error: '" << $2.name << "' function has conflicting declaration ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				functionTable[$2.name].isDefined = 1;
				functionTable[$2.name].startLineImplementation = buffer.nextQuad();
				buffer.backpatch(functionTable[$2.name].callingLines, buffer.nextQuad());
			}
			dotsFunc = 0;
			vaArgsNumOffset = 0;
			symbolTable.clear();
			offset = 0;
			usedRegsInts = 3;
			usedRegsFloats = 0;
			for(int i = 0; i < $4.paramIds.size(); i++){
				Symbol s = Symbol();
				s.type = $4.paramTypes[i];
				s.offset = offset;
				offset += 4;
				symbolTable[$4.paramIds[i]] = s;
			}
		}
| TYPE id_token '(' FUNC_ARGLIST ',' dots_token ')' 
		{
			if(functionTable.find($2.name) == functionTable.end()){
				if ($4.paramIds.size() == 0){
					cerr << "Semantic error: '" << $2.name << "' function has '...' and no arguments ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				if ($4.paramTypes.back() != int_){
					cerr << "Semantic error: '" << $2.name << "' function's last argument isn't an integer ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				for (size_t i = 0; i < $4.paramIds.size(); ++i){
        			for (size_t j = i + 1; j < $4.paramIds.size(); ++j){
            			if ($4.paramIds[i] == $4.paramIds[j]){
							cerr << "Semantic error: '" << $2.name << "' function has two veriebles with the same name  ";
							cerr << "in line number " << to_string(yylineno) << endl;
							exit(0);
						}	
					}
				}
				Function f;
				f.isDefined = 0;
				f.startIndexOptionalParams = $4.paramIds.size();
				f.numOptionalParams = 0;
				f.returnType = $1.type;
				f.paramTypes = $4.paramTypes;
				f.paramIds = $4.paramIds;
				f.startLineImplementation = buffer.nextQuad();
				functionTable[$2.name] = f;
			}
			else{
				if(functionTable[$2.name].isDefined == 1){
					cerr << "Semantic error: '" << $2.name << "' function has multiple definitions ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				if ($4.paramIds.size() == 0){
					cerr << "Semantic error: '" << $2.name << "' function has conflicting declaration ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				if(functionTable[$2.name].startIndexOptionalParams != $4.paramIds.size()){
					cerr << "Semantic error: '" << $2.name << "' function has conflicting declaration ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				if (functionTable[$2.name].paramTypes != $4.paramTypes) {
					cerr << "Semantic error: '" << $2.name << "' function has conflicting declaration ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				if (functionTable[$2.name].paramIds != $4.paramIds) {
					cerr << "Semantic error: '" << $2.name << "' function has conflicting declaration ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				if (functionTable[$2.name].returnType != $1.type) {
					cerr << "Semantic error: '" << $2.name << "' function has conflicting declaration ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				functionTable[$2.name].isDefined = 1;
				functionTable[$2.name].startLineImplementation = buffer.nextQuad();
				buffer.backpatch(functionTable[$2.name].callingLines, buffer.nextQuad());
			}
			dotsFunc = 1;
			vaArgsNumOffset = ($4.paramIds.size() - 1)*4;
			symbolTable.clear();
			offset = 0;
			usedRegsInts = 3;
			usedRegsFloats = 0;
			for(int i = 0; i < functionTable[$2.name].startIndexOptionalParams; i++){
				Symbol s = Symbol();
				s.type = $4.paramTypes[i];
				s.offset = offset;
				offset += 4;
				symbolTable[$4.paramIds[i]] = s;
			}
		}




CALL : id_token '(' CALL_ARGS ')'
		{
			if (functionTable.find($1.name) == functionTable.end()){
				cerr << "Semantic error: Function '" + $1.name + "' is not defined or declared ";
				cerr << "in line number " << to_string(yylineno) << endl;
				exit(0);
			}
			Function f = functionTable[$1.name];
			if(f.startIndexOptionalParams == 0){ // no VA_args
				if($3.paramTypes != f.paramTypes){
					cerr << "Semantic error: Function '" + $1.name + "' recieved wrong arguments ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
			} else{
				if (f.paramTypes.size() > $3.paramTypes.size()) {
					cerr << "Semantic error: Function '" + $1.name + "' recieved wrong arguments ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				int flag = 1;
				for (size_t i = 0; i < f.paramTypes.size(); ++i) {
					if(f.paramTypes[i] != $3.paramTypes[i]){
						flag = 0;
					}
				}
				if(!flag){
					cerr << "Semantic error: Function '" + $1.name + "' recieved wrong arguments ";
					cerr << "in line number " << to_string(yylineno) << endl;
					exit(0);
				}
				buffer.emit("SUBTI I" + intToString(usedRegsInts) + " I" + intToString($3.paramRegs[f.startIndexOptionalParams - 1]) + " " + intToString($3.paramTypes.size() - f.startIndexOptionalParams));
				haltList.push_back(buffer.nextQuad()); 
				buffer.emit("BNEQZ I" + intToString(usedRegsInts) + " ");
			}
			for(int i = 3; i < usedRegsInts; i++){
				buffer.emit("STORI I" + intToString(i) + " I2 " + intToString((i-3)*4));
			}
			int floatOffset = (usedRegsInts - 3) * 4;
			
			for(int i = 0; i < usedRegsFloats; i++){
				buffer.emit("STORF F" + intToString(i) + " I2 " + intToString(i*4 + floatOffset));
			}
			int currOffset = floatOffset + usedRegsFloats*4;
			buffer.emit("STORI I0 I2 " + intToString(currOffset));
			buffer.emit("STORI I1 I2 " + intToString(currOffset + 4));
			buffer.emit("STORI I2 I2 " + intToString(currOffset + 8));
			currOffset += 12;

			int extraOffset = 0;
			if(f.startIndexOptionalParams != 0){
				int vaArgs = ($3.paramTypes.size() - f.startIndexOptionalParams)*4;
				extraOffset = vaArgs;
				for(int i = f.startIndexOptionalParams; i < $3.paramTypes.size(); i++){
					if($3.paramTypes[i] == int_){
						buffer.emit("LOADI I3 I2 " + intToString($3.paramRegs[i]*4));
						buffer.emit("STORI I3 I2 " + intToString(currOffset + vaArgs - 4));
						vaArgs -= 4;
					} else{ // float case
						buffer.emit("LOADF F3 I2 " + intToString(floatOffset + $3.paramRegs[i]*4));
						buffer.emit("STORF F3 I2 " + intToString(currOffset + vaArgs - 4));
						vaArgs -= 4;
					}
				} 
			}

			int args = 0;
			if(f.startIndexOptionalParams == 0){
				args = $3.paramTypes.size();
			} else{
				args = f.startIndexOptionalParams;
			}
			int argsOffset = 0;
			for(int i = 0; i < args; i++){
				if($3.paramTypes[i] == int_){
					buffer.emit("LOADI I3 I2 " + intToString($3.paramRegs[i]*4));
					buffer.emit("STORI I3 I2 " + intToString(currOffset + extraOffset + argsOffset));
					argsOffset += 4;
				} else{ // float case
					buffer.emit("LOADF F3 I2 " + intToString(floatOffset + $3.paramRegs[i]*4));
					buffer.emit("STORF F3 I2 " + intToString(currOffset + extraOffset + argsOffset));
					argsOffset += 4;
				}
			} 
			buffer.emit("ADD2I I1 I2 " + intToString(currOffset + extraOffset));
			buffer.emit("ADD2I I2 I1 " + intToString(argsOffset));

			if(f.isDefined){
				buffer.emit("JLINK " + intToString(f.startLineImplementation));
			} else{ // The function isn't defined yet
				functionTable[$1.name].callingLines.push_back(buffer.nextQuad()); 
				buffer.emit("JLINK ");
			}
			buffer.emit("LOADI I2 I1 " + intToString(-extraOffset - 4));
			currOffset -= 12;
			buffer.emit("LOADI I1 I2 " + intToString(currOffset + 4));
			buffer.emit("LOADI I0 I2 " + intToString(currOffset));
			if(f.returnType == int_){
				$$.RegNum = usedRegsInts;
				buffer.emit("COPYI I3 I" + intToString(usedRegsInts));
			} else if(f.returnType == float_){
				$$.RegNum = usedRegsFloats;
				buffer.emit("COPYF F0 F" + intToString(usedRegsInts));
			}
			currOffset -= usedRegsFloats*4;
			for(int i = 0; i < usedRegsFloats; i++){
				buffer.emit("LOADF F" + intToString(i) + " I2 " + intToString(i*4 + currOffset));
			}
			for(int i = 3; i < usedRegsInts; i++){
				buffer.emit("LOADI I" + intToString(i) + " I2 " + intToString((i-3)*4));
			}
			if(f.returnType == int_){
				usedRegsInts++;
			} else if(f.returnType == float_){
				usedRegsFloats++;
			}
			$$.type = f.returnType;
		}
	;
	
	VA_MATERIALISE : va_token '(' TYPE ',' EXP ')'
		{
			if(dotsFunc == 0){
				cerr << "Semantic error: Can't use va_arg in this function ";
				cerr << "in line number " << to_string(yylineno) << endl;
				exit(0);
			}
			if($3.type == void_t){
				cerr << "Semantic error: Can't have void argument ";
				cerr << "in line number " << to_string(yylineno) << endl;
				exit(0);
			}
			if($5.type != int_){
				cerr << "Semantic error: Argument number has to be an integer ";
				cerr << "in line number " << to_string(yylineno) << endl;
				exit(0);
			}
			buffer.emit("LOADI I" + intToString(usedRegsInts) + " I1 " + intToString(vaArgsNumOffset));
			buffer.emit("SGRTI I" + intToString(usedRegsInts) + " I" + intToString(usedRegsInts) + " I" + intToString($5.RegNum));
			haltList.push_back(buffer.nextQuad()); 
			buffer.emit("BREQZ I" + intToString(usedRegsInts) + " ");
			buffer.emit("MULTI I" + intToString(usedRegsInts) + " -4 I" + intToString($5.RegNum));
			buffer.emit("SUBTI I" + intToString(usedRegsInts) + " 4");
			$$.type = $3.type;
			if($3.type == int_){
				buffer.emit("LOADI I" + intToString(usedRegsInts) + " I1 I" + intToString(usedRegsInts));
				$$.RegNum = usedRegsInts;
				usedRegsInts++;
			} else { // float case
				buffer.emit("LOADI F" + intToString(usedRegsFloats) + " I1 I" + intToString(usedRegsInts));
				$$.RegNum = usedRegsFloats;
				usedRegsFloats++;
			}
		}
	;