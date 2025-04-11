   if (functionTable.find($1.name) == functionTable.end()){
        printSemanticError("Function '" + $1.name + "' was not declared");
    }
    Function currfunc = functionTable[$1.name];

    if(!currfunc.startIndexOptionalParams){ // If startIndexOptionalParams == 0, then this is not a variadic function
        if($3.paramTypes != currfunc.paramTypes){
            printSemanticError("Function '" + $1.name + "' recieved mismatching argument types");
        }
    } else{
        if (currfunc.paramTypes.size() > $3.paramTypes.size()) { // a variadic function
            printSemanticError("Function '" + $1.name + "' recieved mismatching number of arguments");
        }
        for (int i = 0; i < currfunc.paramTypes.size(); ++i) {
            if(currfunc.paramTypes[i] != $3.paramTypes[i]){
                printSemanticError("Function '" + $1.name + "' recieved mismatching argument types");
            }
        }
        int currNumOfVaArgs = $3.paramTypes.size() - currfunc.startIndexOptionalParams;
        int declaredNumOfVaArgs = $3.paramRegs[currfunc.startIndexOptionalParams - 1];
        // Hold '0' in the last register if the number of VaArgs fit.
        buffer.emit("SUBTI I" + intToString(currentScopeRegsNumInt) + " I" + intToString(declaredNumOfVaArgs) + " " + intToString(currNumOfVaArgs));
        runTimeErrorList.push_back(buffer.nextQuad()); 
        // Create an empty goto, to be filled later with run time error handler.
        buffer.emit("BNEQZ I" + intToString(currentScopeRegsNumInt) + " ");
    }
    for(int i = 3; i < currentScopeRegsNumInt; i++){
        buffer.emit("STORI I" + intToString(i) + " I2 " + intToString((i-3)*4));
    }
    int floatOffset = (currentScopeRegsNumInt - 3) * 4;
    
    for(int i = 0; i < currentScopeRegsNumFloat; i++){
        buffer.emit("STORF F" + intToString(i) + " I2 " + intToString(i*4 + floatOffset));
    }
    int currOffset = floatOffset + currentScopeRegsNumFloat*4;
    buffer.emit("STORI I0 I2 " + intToString(currOffset));
    buffer.emit("STORI I1 I2 " + intToString(currOffset + 4));
    buffer.emit("STORI I2 I2 " + intToString(currOffset + 8));
    currOffset += 12;

    int extraOffset = 0;
    if(currfunc.startIndexOptionalParams != 0){
        int vaArgs = ($3.paramTypes.size() - currfunc.startIndexOptionalParams)*4;
        extraOffset = vaArgs;
        for(int i = currfunc.startIndexOptionalParams; i < $3.paramTypes.size(); i++){
            if($3.paramTypes[i] == int_){
                // buffer.emit("LOADI I3 I2 " + intToString($3.paramRegs[i]*4)); // No need for load, we can 
                buffer.emit("STORI I"+ intToString($3.paramRegs[i]) + " I2 " + intToString(currOffset + vaArgs - 4));
                vaArgs -= 4;
            } else{ // float case
                buffer.emit("LOADF F3 I2 " + intToString(floatOffset + $3.paramRegs[i]*4));
                buffer.emit("STORF F3 I2 " + intToString(currOffset + vaArgs - 4));
                vaArgs -= 4;
            }
        } 
    }

    int args = 0;
    if(currfunc.startIndexOptionalParams == 0){
        args = $3.paramTypes.size();
    } else{
        args = currfunc.startIndexOptionalParams;
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

    if(currfunc.isDefined){ // if the function is defined, we know where to jump.
        buffer.emit("JLINK " + intToString(currfunc.startLineImplementation));
    } else{ // if the function isn't defined, we add an empty goto, and fill it with the function definition line later.
        functionTable[$1.name].callingLines.push_back(buffer.nextQuad()); 
        buffer.emit("JLINK ");
    }
    // Retruned from the function, need to handle 
    currOffset -= 12; // updating the offset from I2 to fit the start of the saved registers.
    buffer.emit("LOADI I2 I1 " + intToString(-extraOffset - 4));
    buffer.emit("LOADI I1 I2 " + intToString(currOffset + 4));
    buffer.emit("LOADI I0 I2 " + intToString(currOffset));

    if(currfunc.returnType == int_){ // Return Value 
        $$.RegNum = currentScopeRegsNumInt;
        buffer.emit("COPYI I3 I" + intToString(currentScopeRegsNumInt));
    } else if(currfunc.returnType == float_){
        $$.RegNum = currentScopeRegsNumFloat;
        buffer.emit("COPYF F0 F" + intToString(currentScopeRegsNumFloat));
    }
    currOffset -= currentScopeRegsNumFloat*4;
    for(int i = 0; i < currentScopeRegsNumFloat; i++){
        buffer.emit("LOADF F" + intToString(i) + " I2 " + intToString(i*4 + currOffset));
    }
    for(int i = 3; i < currentScopeRegsNumInt; i++){
        buffer.emit("LOADI I" + intToString(i) + " I2 " + intToString((i-3)*4));
    }
    if(currfunc.returnType == int_){
        currentScopeRegsNumInt++;
    } else if(currfunc.returnType == float_){
        currentScopeRegsNumFloat++;
    }
    $$.type = currfunc.returnType;