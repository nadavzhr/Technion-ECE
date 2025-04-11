		string funcName = $1.name;

		// Check if function name was declared
		if (functionTable.find(funcName) == functionTable.end()) {
			printSemanticError("use of unknown function '" + funcName + "'");
		}

		// Check if number of arguments is correct
		if (functionTable[funcName].paramTypes.size() != $3.paramTypes.size()) {
			printSemanticError("incorrect number of arguments of function '" + funcName + "'");
		}

		//check arguments type compatibility
		for (int i = 0; i < functionTable[funcName].paramTypes.size(); i++) {
			if (functionTable[funcName].paramTypes[i] != $3.paramTypes[i]) {
				printSemanticError("incompatible type of argument in function '" + funcName + "'");
			}
		}
		
		int shiftSize = 0;
		//Allign offset to memory line (4 bytes)
		if (currentScopeOffset%4 != 0) {
			shiftSize = 4 - (currentScopeOffset%4);
			currentScopeOffset += shiftSize;
			buffer->emit("ADD2I I2 I2 " + intToString(shiftSize));
		}
		// Save all registers to memory
		int offset = 0;
		for (int i = 0; i < currentScopeRegsNumInt; ++i) {
			buffer->emit("STORI I" + intToString(i) + " I2 " + intToString(offset));
			offset += 4;
		}
		for (int i = 0; i < currentScopeRegsNumFloat; ++i) {
            // TODO: Check if we need to use F2 instead of I2.
			buffer->emit("STORF F" + intToString(i) + " I2 " + intToString(offset));
			offset += 4;
		}

		// Add 4 bytes for storing called function return value
		offset += 4;

		int paramsCurrentOffset = -4;
		int paramCount = $3.paramRegs.size(); // TOD: need to update for support VaArgs
        int extraParams = paramCount - functionTable[funcName].paramTypes.size() ;
		vector<Type> paramTypes = $3.paramTypes;
		vector<int> paramRegs = $3.paramRegs;
		vector<int> paramOffset;

		$3.paramRegs.clear();
		$3.paramTypes.clear();

		// Calculate the offset in memory to store each parameter of the called function
		for (int i = 0; i < paramCount; i++) {
            offset += 4;
            paramsCurrentOffset -=4;
			paramOffset.push_back(paramsCurrentOffset);
		}

		// Update I2 to the current memory size. update global offset
		buffer->emit("ADD2I I2 I2 " + intToString(offset));
		currentScopeOffset += offset;

		// Set I1 = I2 
		buffer->emit("COPYI I1 I2");
	
		// Store calling function parameters in stack according to each parameter's offset in memory
		for (int i = 0; i < paramCount; i++) {
            if (paramTypes[i] == int_) {
			buffer->emit("STORI"+ " I" + intToString(paramRegs[i]) + " I1 " + intToString(paramOffset[i]));
            } else {
            buffer->emit("STORF"+ " F" + intToString(paramRegs[i]) + " I1 " + intToString(paramOffset[i]));
            }
		}
		
		// Execute JUMP and LINK

		// Add the current line number to the calling lines list of the current function
		functionTable[funcName].startLineImplementation.push_back(buffer->nextquad());
		
		buffer->emit("JLINK "); 
		
		// Back from function, set the stack frame I2 = I1
		buffer->emit("COPYI I2 I1");
		
		// Set the return value to a register
		$$.type = functionTable[funcName].returnType;
		$$.RegNum = currentScopeRegsNum;

        if ($$.type == int_) { // load the return value from the stack
            buffer->emit("LOADI I" + intToString($$.RegNum) + " I1 -4");
        } else if ($$.type == float_) {
            buffer->emit("LOADF F" + intToString($$.RegNum) + " I1 -4");
        }

        // Restore caller I2 value (close the stack frame)
		buffer->emit("SUBTI I2 I2 " + intToString(offset)); 
		
		// Restore all registers from memory
		offset = 0;
		
		for (int i = 0 ; i < currentScopeRegsNumInt ; i++) {
			if (i == 2) { // skip I2 because we already restored it
				offset += 4;
				continue;
			}
			buffer->emit("LOADI I" + intToString(i) + " I2 " + intToString(offset));
			offset += 4;
		}
		for (int i = 0 ; i < currentScopeRegsNumFloat ; i++) {
			buffer->emit("LOADF F" + intToString(i) + " I2 " + intToString(offset));
			offset += 4;
		}

		currentScopeRegsNum++;	