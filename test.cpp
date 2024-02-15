
int GetZeroRuns_InWord(int zeroRuns[SIZE_KEY][2], int signPNB[SIZE_KEY])
{
	int flagDetails = 0;

	if (flagDetails == 1)
	{
		for (int i = 0; i < SIZE_KEY; i++)
		{
			printf("%d", signPNB[i]);
			if ((i + 1) % SIZE_WORD == 0)
				printf("\n");
		}
	}

	int numZeorRuns = 0;

	for (int i = 0; i < 8; i++)
	{
		// first get the run length code in the current word
		int runLengthCode_word[SIZE_WORD][2];
		int numRuns_word = 0;
		int runLength = 0;
		int currentChar = signPNB[(i * SIZE_WORD) + 0];
		for (int j = 0; j < SIZE_WORD; j++)
		{
			int indexKeyBit = i * SIZE_WORD + j;

			if (signPNB[indexKeyBit] == currentChar)
				runLength++;
			else
			{
				runLengthCode_word[numRuns_word][0] = currentChar;
				runLengthCode_word[numRuns_word][1] = runLength;
				numRuns_word++;

				currentChar = signPNB[indexKeyBit];
				runLength = 1;
			}
		}
		runLengthCode_word[numRuns_word][0] = currentChar;
		runLengthCode_word[numRuns_word][1] = runLength;
		numRuns_word++;
		if (flagDetails == 1)
			printf("numRuns_word is %d, run runLength code:\n", numRuns_word);

		// then get the zero-runs in the run length code of current word
		int index_word = 0;
		for (int j = 0; j < numRuns_word; j++)
		{
			if (flagDetails == 1)
				printf("(%d,%d) ", runLengthCode_word[j][0], runLengthCode_word[j][1]);
			if (runLengthCode_word[j][0] == 0)
			{
				zeroRuns[numZeorRuns][0] = i * SIZE_WORD + index_word;
				zeroRuns[numZeorRuns][1] = runLengthCode_word[j][1];

				numZeorRuns++;
			}

			index_word += runLengthCode_word[j][1];
		}
		if (flagDetails == 1)
			printf("\n");
		assert(index_word == SIZE_WORD);
	}

	return numZeorRuns;
}


void DetermineSyncopatedSegment(int signPNB[SIZE_KEY], int signPNB_syncopation[SIZE_KEY], int numSyncopatedSegments[2], int theta[2], int indexRestrictedSegments[SIZE_KEY][2], int syncopatedSegments[SIZE_KEY][2])
{
	/*In the following, the positions only work for ChaCha7, we identify it by "for ChaCha7".*/
	int flagDetails = 0;
	// int flagDetails=1;

	for (int ik = 0; ik < SIZE_KEY; ik++)
		signPNB_syncopation[ik] = signPNB[ik];

	// First, find all the possible zero run-lengths word by word
	int zeroRuns[SIZE_KEY][2];
	int numZeorRuns = GetZeroRuns_InWord(zeroRuns, signPNB);

	// Second, determine the syncopated segments and restricted segments (in the secret key) where we apply the syncopation technique
	if (flagDetails == 1)
		printf("		-- the syncopated segments of secret key where the syncopation technique are applied: \n");
	numSyncopatedSegments[0] = 0; // numSyncopatedSegments_secret_key
	numSyncopatedSegments[1] = 0; // and numSyncopatedSegments_all
	theta[0] = 0;				  // number of syncopated segments with two-bit restriction/condition
	theta[1] = 0;				  // number of syncopated segments with one-bit restriction/condition
	for (int i = 0; i < numZeorRuns; i++)
	{
		// No need to apply the syncopation technique in the begining
		if (zeroRuns[i][0] % SIZE_WORD != 0)
		{
			if (zeroRuns[i][1] >= 2)
			{
				if (((zeroRuns[i][0] + 2) - (zeroRuns[i][0] / SIZE_WORD) * SIZE_WORD) < SIZE_WORD)
				{ // exclue the case we obtain no grain by imposing two-bit restrictions/conditions
					/*Still impose one-bit restriction/condition in this case,
						One more key bit is used to update the value instead of filtering the data.*/
					signPNB_syncopation[zeroRuns[i][0]] = 2;
					// index for restricted segments of key bits in state
					indexRestrictedSegments[numSyncopatedSegments[0]][0] = 1;
					indexRestrictedSegments[numSyncopatedSegments[0]][1] = 128 + zeroRuns[i][0];
					syncopatedSegments[numSyncopatedSegments[0]][0] = 128 + zeroRuns[i][0] + 1;
					syncopatedSegments[numSyncopatedSegments[0]][1] = zeroRuns[i][1] - 1;

					numSyncopatedSegments[0]++;
					// for ChaCha7
					if (zeroRuns[i][0] / SIZE_WORD < 4)
					{
						theta[1]++;
					}
					else
					{
						theta[1] += 3;
					}
					if (zeroRuns[i][0] / SIZE_WORD < 4)
						numSyncopatedSegments[1]++;
					else
						numSyncopatedSegments[1] += 3;

					// signPNB_syncopation[zeroRuns[i][0]]=2;
					// signPNB_syncopation[zeroRuns[i][0]+1]=2;
					// //index for restricted segments of key bits in state
					// indexRestrictedSegments[numSyncopatedSegments[0]][0]=2;
					// indexRestrictedSegments[numSyncopatedSegments[0]][1]=128+zeroRuns[i][0];
					// 	syncopatedSegments[numSyncopatedSegments[0]][0]=128+zeroRuns[i][0]+2;
					// 	syncopatedSegments[numSyncopatedSegments[0]][1]=zeroRuns[i][1]-2;

					// numSyncopatedSegments[0]++;
					// //for ChaCha7
					// 	if(zeroRuns[i][0]/SIZE_WORD<4){
					// 		theta[0]++;
					// 	}
					// 	else {
					// 		theta[0]+=3;
					// 	}
					// if(zeroRuns[i][0]/SIZE_WORD<4) numSyncopatedSegments[1]++;
					// else numSyncopatedSegments[1]+=3;
				}
				else
				{ // impose one-bit restriction/condition in this case
					signPNB_syncopation[zeroRuns[i][0]] = 2;
					// index for restricted segments of key bits in state
					indexRestrictedSegments[numSyncopatedSegments[0]][0] = 1;
					indexRestrictedSegments[numSyncopatedSegments[0]][1] = 128 + zeroRuns[i][0];
					syncopatedSegments[numSyncopatedSegments[0]][0] = 128 + zeroRuns[i][0] + 1;
					syncopatedSegments[numSyncopatedSegments[0]][1] = zeroRuns[i][1] - 1;
					// printf("		-- '+2' is beyond the current word, and new segment is (%d,%d).\n",\
						// syncopatedSegments[numSyncopatedSegments[0]][0],\
						// syncopatedSegments[numSyncopatedSegments[0]][1]);

					numSyncopatedSegments[0]++;
					// for ChaCha7
					if (zeroRuns[i][0] / SIZE_WORD < 4)
					{
						theta[1]++;
					}
					else
					{
						theta[1] += 3;
					}
					if (zeroRuns[i][0] / SIZE_WORD < 4)
						numSyncopatedSegments[1]++;
					else
						numSyncopatedSegments[1] += 3;
				}
			}
			else
			{ // zeroRuns[i][1]=1, and can only impose one-bit restriction/condition in this case
				if (((zeroRuns[i][0] + 1) - (zeroRuns[i][0] / SIZE_WORD) * SIZE_WORD) < SIZE_WORD)
				{ // exclue the case we obtain no grain by imposing one-bit restrictions/conditions
					signPNB_syncopation[zeroRuns[i][0]] = 2;
					// index for restricted segments of key bits in state
					indexRestrictedSegments[numSyncopatedSegments[0]][0] = 1;
					indexRestrictedSegments[numSyncopatedSegments[0]][1] = 128 + zeroRuns[i][0];
					syncopatedSegments[numSyncopatedSegments[0]][0] = 128 + zeroRuns[i][0] + 1;
					syncopatedSegments[numSyncopatedSegments[0]][1] = zeroRuns[i][1] - 1;

					numSyncopatedSegments[0]++;
					// for ChaCha7
					if (zeroRuns[i][0] / SIZE_WORD < 4)
					{
						theta[1]++;
					}
					else
					{
						theta[1] += 3;
					}
					if (zeroRuns[i][0] / SIZE_WORD < 4)
						numSyncopatedSegments[1]++;
					else
						numSyncopatedSegments[1] += 3;
				}
				// else{
				// printf("		-- '+1' is beyond the current word.\n");
				// }
			}
			if (flagDetails == 1)
				if (numSyncopatedSegments[0] > 0)
					printf("			--> %d-th restricted position of secret key (%d-bit restricted segment), at %d, and segment is (%d,%d)\n",
						   numSyncopatedSegments[0], indexRestrictedSegments[numSyncopatedSegments[0] - 1][0],
						   indexRestrictedSegments[numSyncopatedSegments[0] - 1][1] - 128,
						   syncopatedSegments[numSyncopatedSegments[0] - 1][0] - 128,
						   syncopatedSegments[numSyncopatedSegments[0] - 1][1]);
		}
	}
	assert((theta[0] + theta[1]) == numSyncopatedSegments[1]);

	// for ChaCha7
	pos_Max = numSyncopatedSegments[1];					// +++++ for cutting Conditions +++++
	pos_gap = (pos_Max - numSyncopatedSegments[0]) / 2; // +++++ for cutting Conditions +++++

	if (flagDetails == 1)
		printf("		-- total %d syncopation positions for all, and %d for secret key.\n\n", numSyncopatedSegments[1], numSyncopatedSegments[0]);
}