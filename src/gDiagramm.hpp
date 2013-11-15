/* gDiagramm.hpp   */
/* class Diagramm */

#define FIELDINT   0
#define FIELDUINT  1
#define FIELDFLOAT 2
#define FIELDDOUBLE    3
#define FIELDCHAR      4
#define FIELDSTRING    5
#define FIELDDATE      6
#define FIELDTIME      7
#define FIELDDATETIME  8
#define FIELDDATETIME1 9 //second part of datatime (not seen in form!)

//10 шагов = 1 линия разметки - надо сделать так, чтоб в 10 шагов помещалось 24/12/6/3/2/1 час 30/20/10/5/2/1 минута
//т.е. (24/12/6/3/2/1)*60*60/10  секунд
#define TIMESTEP24H (24*60*60)/10
#define TIMESTEP12H (12*60*60)/10
#define TIMESTEP6H  (6*60*60)/10
#define TIMESTEP3H  (3*60*60)/10
#define TIMESTEP2H  (2*60*60)/10
#define TIMESTEP1H  (1*60*60)/10
#define TIMESTEP30M (30*60)/10
#define TIMESTEP20M (20*60)/10
#define TIMESTEP10M (10*60)/10
#define TIMESTEP5M  (5*60)/10
#define TIMESTEP2M  (2*60)/10
#define TIMESTEP1M  (1*60)/10


class Field
{  public:
      int type; // field type
//тип поля:
//integer          0 (d,o,x,X)
//Unsigned integer 1 (u)
//Floating-point   2 (e,f,g,E,G)
//Character        3 (c)
//String           4 (s)
//double           5 (le,lf,lg,lE,lG)
      int use;        //use=1, no=0
      int num;        // field number
      char name[128]; // имя поля
      int color;      // plot RGB color, -1 = rand
      double minf,maxf; // min/max of data
      double amplification;   //  коэффициент усиления
      double shift;   //  сдвиг
      int markType;   // (не=0)отмечать отдельные точки
      int markColor;  // цвет отметки
      int vectorType; // 0, иначе поле есть координата ветора: 1 - начало первого X,  2 начало первого Y, 3 - конец первого X, 4 - конец первого Y, 5  о второго
      time_t time_min; // минимальное время для DATATIME поля
    Field(void)
    {  name[0] = 0;
       use = 0;
       num = 0;
       type = FIELDSTRING;
       color = -1;
       minf = maxf = 0.;
       markType = 0;
       markColor=-1;
       amplification = 1.;
       shift = 0.;
       vectorType = 0;
       time_min = 0;
    }
/* запись запоминаемых параметров */
   int Write(FILE *fp)
   {  char prefix[20]="field.";
      sprintf(prefix,"field%i.",num);
      fprintf(fp,"%sname=%s\n", prefix,name);
      if(use > 1)
      {   fprintf(fp,"%suse=1\n",  prefix);
          fprintf(fp,"%suseAsX=1\n",  prefix);
      } else {
          fprintf(fp,"%suse=%i\n",  prefix,use);
      }
      fprintf(fp,"%stype=%i\n", prefix,type);
      fprintf(fp,"%scolor=%x\n", prefix,color);
      if(markType)
      {    fprintf(fp,"%smarkType=%i\n", prefix,markType);
           fprintf(fp,"%smarkColor=%i\n", prefix,markColor);
      }
      fprintf(fp,"%samplification=%f\n", prefix,amplification);
      fprintf(fp,"%sshift=%f\n", prefix,shift);
      return 0;
   }

   int AnalizeRecodrRead(char *parname, char *par)// анализ прочитанной строки конфига
   {
      int i,is=0,npar=0;
      static char *psNames[]=
        { "num","name","use","color","markType","markColor","useAsX","amplification","shift","vectorType",
          NULL };

      for(i=0;psNames[i];i++)
      {   if(!strcmp(parname,psNames[i]))
          {  is = 1;
             npar = i;
             break;
          }
      }
      if(!is) return 1;
      switch(npar)
      {  case 0:
           sscanf(par,"%d", &num);
          break;
         case 1:
           strncpy(name,par,sizeof(name)-1);
           name[sizeof(name)-1] = 0;
            break;
         case 2:
             sscanf(par,"%d", &i);
             if(i && use>1) use = 2;
             else use = i;
           break;
         case 3:
           sscanf(par,"%x", &color);
           break;
         case 4:
           sscanf(par,"%x", &markType);
            break;
         case 5:
           sscanf(par,"%x", &markColor);
            break;
         case 6:
           sscanf(par,"%x", &i);
           if(i > 0) use = 2;
            break;
         case 7:
           sscanf(par,"%lf", &amplification);
            break;
         case 8:
           sscanf(par,"%lf", &shift);
            break;
         case 9:
           sscanf(par,"%d", &vectorType);
            break;
      }
      return 0;
   }

};


class Diagramm
{  public:
      int nfields;          /* число полей */
      int nfieldsAlloc;     /* memory allocated for */
      class Field *pfields; /* указатель на nfields * class Field */
      int  *offsets;  /* массив длиной nfields со смещениями полей данных */
      char *databuff; /* буфер данных */
      int  Lendatabuf;/* длина буфера данных / длина файла данных для режима BigFile */
      int  recordLen; /* длина записи в буфере данных */
      int  NumRec;    /* число записей */
      char * *strptrbuf; /* буфер указателей на память для временных строковых переменных, длиной nfields (для простоты)*/
      char *strBuff;     /* буфер для строк */
      char *pstrBuff;    /* указатель на свободное место в буфере строк */
      int  LenstrBuff;   /* длина буфера для строк */
      char FormatStr[256];   /* format string */
      char FormName[256];    /* Form's file name */
      char AppName[256];     /* Application Name */
      char FileOut[256];     /* Output filename  */
      char FileData[256];    /* файл данных  */
      char Title[80];        /* Title */
      int nfieldUsedAsX;     /* index of field, used as X */
      double MinX,MaxX,MinY,MaxY;
      double mashtX, mashtY;
      int DimX,DimY;     /* sizeof diagramm in pixels*/
      int BackColor;     /* Background Color */
      int TitleColor;    /* Title Color */
      int TitleFont;     /* Title Font 0-4 */
      int AxesColor;     /* Axes Color */
      int AxesNumColor;  /* Axes Numbers Color */
      int AxesNumFont;   /* Axes Numbers Font */
/* Grid */
      int GridColor;     /* grid color       */
      int GridXon;
      int GridYon;
      double GridXStart, GridXStep;
      double GridYStart, GridYStep;
      char FormatX[80];   /* format string for X axis */
      char FormatY[80];   /* format string for Y axis */
      int LegendUse;       /* Legend */
      int LegendStartX;
      int LegendStartY;

      int isDataTimeX;  /* ось X - время,  надо делить на сутки-часы-минуты =2 - рисуем в 2 строки время/дату =1 - рисуем только дату */
      int showDataTimeXseconds;  /* если ось X - время, то надо ли показывать на оси секунды */

      int useLocale;
      char Locale[80];
      int BigFile; //=0, если =1 - данные не хранить в памяти, а каждый раз читать построчно
      int BigFileRec;
      int startLine; //при обработке части файла данных
      int endLine;   //при обработке части файла данных
      int startLineSeek; //указатель в файле на начало данных поиска, если startLine не 0
      int endLineSeek; //указатель в файле на конец данных поиска, нужно для определения длины буфера
      
      FILE *fp;

   Diagramm(void)
   {  nfields = 0;
      pfields = NULL;
      offsets = NULL;
      nfieldsAlloc = 0;
      databuff = NULL;
      strptrbuf = NULL;
      Lendatabuf = 0;
      recordLen = 0;
      LenstrBuff = 0;
      strBuff = NULL;
      NumRec = 0;
      Title[0] = 0;
      BackColor  = 0xb0b0b0;
      GridColor  = 0x808080;
      TitleColor = 0xa00000;
      TitleFont  = 2;
      AxesColor = 0x303030;
      AxesNumColor = 0xa02020;
      AxesNumFont=1;
      DimX = DimY = 64;
      mashtX = mashtY = 1.;
      GridXon = GridYon = 1;
      GridXStart = 0.;
      GridXStep = 1.;
      GridYStart = 0.;
      GridYStep  = 1.;
      nfieldUsedAsX = -1;
      strcpy(FormatX,"%.f");
      strcpy(FormatY,"%.f");
      strcpy(FormatStr,"%f %f");
      strcpy(FileOut,"demo.png");
      strcpy(FileData,"demo.dat");
      LegendUse=1;
      LegendStartX = -1;
      LegendStartY = -1;
      isDataTimeX = 0;
      showDataTimeXseconds = 0;
      useLocale = 0;
      strcpy(Locale,"C");
      BigFile = 0;
      BigFileRec = -1;
      startLine = 0; //при обработке части файла данных
      endLine   = 0;   //при обработке части файла данных
      startLineSeek = 0; //указатель в файле на начало данных поиска, если startLine не 0

      fp = NULL;
   }
   int AddField(int type)
   {  int l;
      class Field tmp;

      if(pfields == NULL)
      {   nfieldsAlloc = 16;
          l = sizeof(class Field);
          pfields = (Field *) calloc(l,nfieldsAlloc);
          offsets = (int *)   calloc(sizeof(int),nfieldsAlloc+1);
          strptrbuf = (char * *) calloc(sizeof(char *),nfieldsAlloc+1);
      } else if(nfieldsAlloc <= nfields+1) {
          nfieldsAlloc += 16;
          pfields = (Field *) realloc((void *)pfields, sizeof(class Field) *nfieldsAlloc);
          offsets  = (int *) realloc((void *)offsets , sizeof(int) *(nfieldsAlloc+1));
          strptrbuf = (char * *) realloc((void *)strptrbuf , sizeof(char *) *(nfieldsAlloc+1));
      }
      tmp.type = type;
      tmp.num  = nfields;
      pfields[nfields] = tmp;

      nfields++;
      return 0;
   }


   int AddData0(int iy, char *str, char *pdata)
   { //  char  *pdata;
       int i, n, n1;
       void *ptr[64];

//       pdata = databuff + iy * recordLen;
       for(i=0;i<nfields;i++)
       {  ptr[i] = (void *)(pdata+offsets[i]);
          if(pfields[i].type >= FIELDSTRING)
          { *((char * *) pdata+offsets[i]) = strptrbuf[i];
            ptr[i] = (void *)(strptrbuf[i]);
          }
       }
    if(nfields <= 4)
    {
       n = sscanf(str,FormatStr,ptr[0],ptr[1],ptr[2],ptr[3]);
    } else
         if(nfields <= 8) {
       n = sscanf(str,FormatStr,ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7]);
    } else
          if(nfields <= 16) {
       n = sscanf(str,FormatStr,ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7],ptr[8],ptr[9],
                                ptr[10],ptr[11],ptr[12],ptr[13],ptr[14],ptr[15]);
    } else
         if(nfields <= 32) {
       n = sscanf(str,FormatStr,ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7],ptr[8],ptr[9],
                                ptr[10],ptr[11],ptr[12],ptr[13],ptr[14],ptr[15],ptr[16],ptr[17],ptr[18],ptr[19],
                                ptr[20],ptr[21],ptr[22],ptr[23],ptr[24],ptr[25],ptr[26],ptr[27],ptr[28],ptr[29],
                                ptr[30],ptr[31]);
    } else  {
       n = sscanf(str,FormatStr,ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7],ptr[8],ptr[9],
                                ptr[10],ptr[11],ptr[12],ptr[13],ptr[14],ptr[15],ptr[16],ptr[17],ptr[18],ptr[19],
                                ptr[20],ptr[21],ptr[22],ptr[23],ptr[24],ptr[25],ptr[26],ptr[27],ptr[28],ptr[29],
                                ptr[30],ptr[31],ptr[32],ptr[33],ptr[34],ptr[35],ptr[36],ptr[37],ptr[38],ptr[39],
                                ptr[40],ptr[41],ptr[42],ptr[43],ptr[44],ptr[45],ptr[46],ptr[47],ptr[48],ptr[49],
                                ptr[50],ptr[51],ptr[52],ptr[53],ptr[54],ptr[55],ptr[56],ptr[57],ptr[58],ptr[59]);
    }
       for(i=0;i<nfields;i++)
       {  ptr[i] = (void *)(pdata+offsets[i]);
          if(pfields[i].type == FIELDSTRING)
          {  strcpy(pstrBuff,strptrbuf[i]);
             *((char * *) pdata+offsets[i]) = pstrBuff;
             pstrBuff += strlen(strptrbuf[i]);
//             printf("%s ",strptrbuf[i]);
             *pstrBuff = 0;
             pstrBuff++;
          } else
                 if(pfields[i].type == FIELDDATE) {
             unsigned int dd =0, dyear,dmonth, dday;
//0xffff0000 - year
//0x0000ff00 - month
//0x000000ff - day
//todo do more flexible date format
             n1 = sscanf(strptrbuf[i],"%d.%d.%d",&dday,&dmonth, &dyear);
             dd = dday |(dmonth<<8)|(dyear<<16);
             *((unsigned int *)ptr[i]) = dd;
          } else
                 if(pfields[i].type == FIELDTIME) {
             unsigned int dd =0, dhour,dmin, dsec;
//0xffff0000 - hour
//0x0000ff00 - min
//0x000000ff - sec
//todo do more flexible time  format ?
             n1 = sscanf(strptrbuf[i],"%d:%d:%d",&dhour,&dmin, &dsec);
             dd = dsec |(dmin<<8)|(dhour<<16);
             *((unsigned int *)ptr[i]) = dd;
          } else
                 if(pfields[i].type == FIELDDATETIME) {
             unsigned int dyear,dmonth, dday;
             unsigned int dhour,dmin, dsec;
             struct tm t0;
             time_t tkonstr;
//todo do more flexible date format
             n1 = sscanf(strptrbuf[i],"%d.%d.%d", &dday,&dmonth, &dyear);
             n1 = sscanf(strptrbuf[i+1],"%d:%d:%d",&dhour,&dmin, &dsec);
             memset(&t0,0, sizeof(tm));
//todo: 4 bytes for time_t only for VAC3.08!!!

             t0.tm_year= dyear; //years since 1900
             if(t0.tm_year > 1000) t0.tm_year -= 1900;
             else if(t0.tm_year < 70) t0.tm_year += 100;
             t0.tm_mon = dmonth-1; //months since January [0-11]
             t0.tm_mday= dday;//day of the month [1-31]
             t0.tm_hour = dhour;
             t0.tm_min = dmin;
             t0.tm_sec = dsec;
             tkonstr = mktime(&t0);
             if(NumRec == 0)
                   pfields[i].time_min = tkonstr;
             if(difftime(tkonstr,pfields[i].time_min) < 0)
                                      pfields[i].time_min = tkonstr;

             *((unsigned int *)ptr[i]) = tkonstr;
             i++;
          }

       }
       NumRec++;
       return 0;
   }

   int AddData(int iy, char *str)
   {   int rc;
       char  *pdata;
       pdata = databuff + iy * recordLen;
       rc = AddData0(iy, str, pdata);
       return rc;
   }
/* построчное чтение данных для очень больших файлов */
   char *GetDataPerStr(int nrec)
   {   static char strdata[512], DataStr[512];
       char str[512];
       char *pstr;
       int i, n;
       if(nrec == BigFileRec)
       {
          pstr = &DataStr[0];
          return  pstr;
       }
       if(nrec < BigFileRec && fp)
       { fclose(fp);
         fp = NULL;
       }

       if(fp == NULL)
       { fp = fopen(FileData,"r");
         if(startLineSeek)
            fseek(fp, startLineSeek,SEEK_SET );
         BigFileRec = -1;
       }
       if(fp == NULL)
            return NULL;
       n = nrec - BigFileRec - 1;
      if(n >  0)
       for(i=0; i < n; i++)
       {  pstr = fgets(str,511,fp);
          if(!pstr)
                  break;
         if(*pstr == ';') i--;
       }

       for(i=0; ; i++)
       {  pstr = fgets(str,511,fp);
          if(!pstr)
                  break;
         if(*pstr == ';') i--;
         else break;
       }

    if(BigFile && ((nrec%10240) == 0) )
    {  printf("Read %i%% \r", (int)(ftell(fp)*100./Lendatabuf) ); fflush(stdout);
    }


       pstrBuff = strBuff = strdata;
       BigFileRec = nrec;

       AddData0(nrec,str,DataStr);
       NumRec--;
       pstr = &DataStr[0];
      return  pstr;
   }

   int CalcMinMax(void)
   {  int i,j, startX=1, startY=1;
       void *ptr[64];
      char  *pdata, cd;
      int id;
      double dd;
      int iGridXStep,nx50,ny50;
      double lgxstep, tmp, k;
      struct tm t0, *pt0;
      time_t tkonstr;

      MinX = 0;
      MaxX = NumRec;
      MinY = MaxY = 0.;
/* определяем, есть ли nfieldUsedAsX */
      for(j=0;j<nfields;j++)
      {   if(!pfields[j].use) continue;
           if(pfields[j].use > 1)
           {  nfieldUsedAsX = j;
              break;
           }
      }

      for(i=0;i<NumRec;i++)
      {
         pdata = databuff + i * recordLen;
         for(j=0;j<nfields;j++)
         {   if(!pfields[j].use) continue;
             dd = GetValue(i,j);

             if(nfieldUsedAsX == j || ((pfields[j].vectorType&0x01) == 1) )
             {  if(startX)
                { MinX = MaxX = dd; startX = 0;
                }
                if(dd < MinX) MinX = dd;
                if(dd > MaxX) MaxX = dd;
             } else {
                if(startY)
                { MinY = MaxY = dd; startY = 0;
                }
                if(dd < MinY) MinY = dd;
                if(dd > MaxY) MaxY = dd;
             }

         }
      }
      if(MinY == MaxY)
      {  tmp =  MaxY;
         if(tmp > 0.)
         { MaxY = tmp * 1.0001;
           MinY = tmp * 0.9999;
         } else if(tmp == 0.) {
            MaxY =  0.0001;
            MinY = -0.0001;
         } else {
           MinY = tmp * 1.0001;
           MaxY = tmp * 0.9999;
         }
      }

      nx50 = (DimX-32)/5;
      ny50 = (DimY-32)/5;

M:       GridXStart = MinX;
         GridXStep = (MaxX-MinX)/nx50;
         if(GridXStep == 0.) GridXStep = 1.;

         if(isDataTimeX && GridXStep > TIMESTEP1M)
         {     showDataTimeXseconds = 0;
               pt0 = localtime(&pfields[nfieldUsedAsX].time_min);
               t0 = *pt0;
               if( GridXStep > TIMESTEP24H)
               {          GridXStep = int(GridXStep)/(TIMESTEP24H)*(TIMESTEP24H); //шаг кратный 24/10 часам
//                         GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP24H)*(TIMESTEP24H); //шаг кратный
                         t0.tm_hour =0;
                         t0.tm_min = 0;
                         t0.tm_sec = 0;
                         isDataTimeX = 1;
               }
               else if( GridXStep > TIMESTEP12H)
               {
                         GridXStep = int(GridXStep)/(TIMESTEP12H)*(TIMESTEP12H); //шаг кратный 12/10 часам
//                         GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP12H)*(TIMESTEP12H);
                         t0.tm_hour = t0.tm_hour/12*12;
                         t0.tm_min = 0;
                         t0.tm_sec = 0;
               }
               else if( GridXStep > TIMESTEP6H)
               {
                         GridXStep = int(GridXStep)/(TIMESTEP6H)*(TIMESTEP6H);
//                       GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP6H)*(TIMESTEP6H);
                         t0.tm_hour = t0.tm_hour/6*6;
                         t0.tm_min = 0;
                         t0.tm_sec = 0;
               }
               else if( GridXStep > TIMESTEP3H)
               {
                         GridXStep = int(GridXStep)/(TIMESTEP3H)*(TIMESTEP3H);
//                       GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP3H)*(TIMESTEP3H);
                         t0.tm_hour = t0.tm_hour/3*3;
                         t0.tm_min = 0;
                         t0.tm_sec = 0;
               }
               else if( GridXStep > TIMESTEP2H)
               {
                         GridXStep = int(GridXStep)/(TIMESTEP2H)*(TIMESTEP2H);
//                       GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP2H)*(TIMESTEP2H);
                         t0.tm_hour = t0.tm_hour/2*2;
                         t0.tm_min = 0;
                         t0.tm_sec = 0;
               }
               else if( GridXStep > TIMESTEP1H)
               {
                         GridXStep = int(GridXStep)/(TIMESTEP1H)*(TIMESTEP1H);
//                         GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP1H)*(TIMESTEP1H);
                         t0.tm_min = 0;
                         t0.tm_sec = 0;
               }
               else if( GridXStep > TIMESTEP30M)
               {
                         GridXStep = int(GridXStep)/(TIMESTEP30M)*(TIMESTEP30M);
//                         GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP30M)*(TIMESTEP30M);
                         t0.tm_min = t0.tm_min/30*30;
                         t0.tm_sec = 0;
               }
               else if( GridXStep > TIMESTEP20M)
               {
                         GridXStep = int(GridXStep)/(TIMESTEP20M)*(TIMESTEP20M);
//                         GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP20M)*(TIMESTEP20M);
                         t0.tm_min = t0.tm_min/20*20;
                         t0.tm_sec = 0;
               }
               else if( GridXStep > TIMESTEP10M)
               {
                         GridXStep = int(GridXStep)/(TIMESTEP10M)*(TIMESTEP10M);
//                         GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP10M)*(TIMESTEP10M);
                         t0.tm_min = t0.tm_min/10*10;
                         t0.tm_sec = 0;
               }
               else if( GridXStep > TIMESTEP5M)
               {
                         GridXStep = int(GridXStep)/(TIMESTEP5M)*(TIMESTEP5M);
                         GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP5M)*(TIMESTEP5M);
                         t0.tm_min = t0.tm_min/5*5;
                         t0.tm_sec = 0;
               }
               else if( GridXStep > TIMESTEP2M)
               {
                         GridXStep = int(GridXStep)/(TIMESTEP2M)*(TIMESTEP2M);
                         GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP2M)*(TIMESTEP2M);
                         t0.tm_min = t0.tm_min/2*2;
                         t0.tm_sec = 0;
               }
               else
               {
                         GridXStep = int(GridXStep)/(TIMESTEP1M)*(TIMESTEP1M);
                         GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP1M)*(TIMESTEP1M);
                         t0.tm_sec = 0;
               }
               tkonstr = mktime(&t0);
               GridXStart = tkonstr - pfields[nfieldUsedAsX].time_min;

              j = (DimX-32)/ ((MaxX-MinX)/GridXStep);  //пикселей на деление
              if(j <= 3)
              {  if(nx50 > 5)
                 {  nx50--;
                    goto M;
                 }
              }


         } else {
            lgxstep = log10(GridXStep);
            iGridXStep = (int) lgxstep;
            if(lgxstep < 0.) iGridXStep--;
            tmp = pow(10.,(double)iGridXStep);

            k = GridXStep / tmp;
            if(k < 1.1) k = 1.;
            else if(k < 2.5) k = 2.;
            else if(k < 6.) k = 5.;
            else k = 10.;

            GridXStep = k * tmp;
            showDataTimeXseconds = 1;
              iGridXStep = int(MinX/tmp * 1000);
              GridXStart = iGridXStep/1000.*tmp;
         }
#if 0
         if(isDataTimeX)
         {    if( GridXStep > TIMESTEP24H)
                      GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP24H)*(TIMESTEP24H); //шаг кратный
              else if( GridXStep > TIMESTEP12H)
                      GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP12H)*(TIMESTEP12H); //шаг кратный
              else if( GridXStep > TIMESTEP6H)
                      GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP6H)*(TIMESTEP6H);
              else if( GridXStep > TIMESTEP3H)
                      GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP3H)*(TIMESTEP3H);
              else if( GridXStep > TIMESTEP2H)
                      GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP2H)*(TIMESTEP2H);
              else if( GridXStep > TIMESTEP1H)
                      GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP1H)*(TIMESTEP1H);
              else if( GridXStep > TIMESTEP30M)
                      GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP30M)*(TIMESTEP30M);
              else if( GridXStep > TIMESTEP20M)
                      GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP20M)*(TIMESTEP20M);
              else if( GridXStep > TIMESTEP10M)
                      GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP10M)*(TIMESTEP10M);
              else if( GridXStep > TIMESTEP5M)
                      GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP5M)*(TIMESTEP5M);
              else if( GridXStep > TIMESTEP2M)
                      GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP2M)*(TIMESTEP2M);
              else
                      GridXStart = int(pfields[nfieldUsedAsX].time_min)/(TIMESTEP1M)*(TIMESTEP1M);
             GridXStart -= pfields[nfieldUsedAsX].time_min;
         }

#endif //0
         GridYStart = MinY;
         GridYStep = (MaxY-MinY)/ny50;
         if(GridYStep == 0.) GridYStep = 1.;

         lgxstep = log10(GridYStep);
         iGridXStep = (int) lgxstep;
         if(lgxstep < 0.) iGridXStep--;
         tmp = pow(10.,(double)iGridXStep);

         k = GridYStep / tmp;
         if(k < 1.6) k = 1.;
//         else if(k < 1.8) k = 1.5;
         else if(k < 2.5) k = 2.;
         else if(k < 4.2) k = 4.;
         else if(k < 6.5) k = 5.;
         else k = 10.;
         GridYStep = k * tmp;
         iGridXStep = int(MinY/(GridYStep*10));
         GridYStart = iGridXStep * GridYStep*10;


      return 0;
   }

   double  GetValue(int nrec, int ifield)
   {  int j;
       void *ptr[64];
      char  *pdata, cd;
      int id;
      float fd;
      double dd=0.;

      if(BigFile)
      {  pdata = GetDataPerStr(nrec);
      } else {
         pdata = databuff + nrec * recordLen;
      }

         j = ifield;
         {
             switch(pfields[j].type)
             {  case FIELDINT:
                    id = *((int *) (pdata + offsets[j]));
                    dd = double(id);
                    if(pfields[j].shift != 0.)
                          dd += pfields[j].shift;
                    if(pfields[j].amplification != 1.)
                          dd *= pfields[j].amplification;
                   break;
                case FIELDUINT:
                    id = *((int *) (pdata + offsets[j]));
                    dd = double(id);
                    if(pfields[j].shift != 0.)
                          dd += pfields[j].shift;
                    if(pfields[j].amplification != 1.)
                          dd *= pfields[j].amplification;
                   break;
                case FIELDFLOAT:
                    fd = *((float *) (pdata + offsets[j]));
                    dd = double(fd);
                    if(pfields[j].shift != 0.)
                          dd += pfields[j].shift;
                    if(pfields[j].amplification != 1.)
                          dd *= pfields[j].amplification;
                   break;
                case FIELDCHAR:
                    cd = *((char *) (pdata + offsets[j]));
                    dd = double(cd);
                    if(pfields[j].shift != 0.)
                          dd += pfields[j].shift;
                    if(pfields[j].amplification != 1.)
                          dd *= pfields[j].amplification;
                   break;
                case FIELDSTRING:
                   break;
                case FIELDDOUBLE:
                    dd = *((double *) (pdata + offsets[j]));
                    if(pfields[j].shift != 0.)
                          dd += pfields[j].shift;
                    if(pfields[j].amplification != 1.)
                          dd *= pfields[j].amplification;
                   break;
                case FIELDDATE:
                 {  unsigned int dd;
                    struct tm t0;
                    time_t tkonstr;
                    dd = *((unsigned int *) (pdata + offsets[j]));
                    memset(&t0,0, sizeof(tm));
//todo
                    id = (dd&0xff);
                    t0.tm_year= dd>>16; //years since 1900
                    if(t0.tm_year > 1000) t0.tm_year -= 1900;
                    else if(t0.tm_year < 70) t0.tm_year += 100;
                    t0.tm_mon    = (dd&0xff00)>>8;
                    t0.tm_mday   = (dd&0xff);

/*
         int tm_sec;    seconds after the minute [0-61]
         int tm_min;    minutes after the hour [0-59]
         int tm_hour;   hours since midnight [0-23]
         int tm_mday;   day of the month [1-31]
         int tm_mon;    months since January [0-11]
         int tm_year;   years since 1900
         int tm_wday;   days since Sunday [0-6]
         int tm_yday;   days since January 1 [0-365]
         int tm_isdst;  Daylight Saving Time flag

*/
                    tkonstr = mktime(&t0);
                    dd = double(tkonstr);
                    if(pfields[j].shift != 0.)
                          dd += pfields[j].shift;
                    if(pfields[j].amplification != 1.)
                          dd *= pfields[j].amplification;
                 }
                   break;

                case FIELDTIME:
                 {  unsigned int dd;
                    struct tm t0;
                    dd = *((unsigned int *) (pdata + offsets[j]));
//todo
                    dd = (dd&0xff) + ((dd&0xff00)+(dd>>16)*24.)*60.;
                    if(pfields[j].shift != 0.)
                          dd += pfields[j].shift;
                    if(pfields[j].amplification != 1.)
                          dd *= pfields[j].amplification;
                 }
                   break;
                case FIELDDATETIME:
                    id = *((int *) (pdata + offsets[j]));
                     dd = difftime(id,pfields[j].time_min);

                    if(pfields[j].shift != 0.)
                          dd += pfields[j].shift;
                    if(pfields[j].amplification != 1.)
                          dd *= pfields[j].amplification;

                   break;

             }
         }
         return dd;
   }

   int PlotData(void);
//   int PlotData(PBYTE pBmpBuffer, int isSame);

   int PlotMark(gdImagePtr im, int x, int y, int type, int color);

   /* Read and analyze  formfile(project) */
   int  Read(char *fname)
   {  int i, id,rc,rc0,nf, NstrAnalyzed=0;
      FILE *fp;
      char str[256],nameClass[128],name[128],par[128], *pstr;

      fp = fopen(fname,"r");
      if(fp == NULL)
         return 1;
      strcpy(FormName,fname);
      do
      {  rc = ReadStr(fp, str,nameClass,name,par);
         if(!strcmpi(nameClass,"form"))
         {    AnalizeRecodrRead(name,par);
              NstrAnalyzed++;
         }  else if(strstr(nameClass,"field")) {

            pstr = nameClass;
            pstr += strlen("field");
            rc0 = sscanf(pstr,"%d", &nf);
            if(rc0 != 1 || nf < 0 || nf >= nfields) nf = 0;
            pfields[nf].AnalizeRecodrRead(name,par);
            NstrAnalyzed++;
         }

      } while(!rc);
      fclose(fp);

      id = 0;
      for(i=0;i<nfields;i++)
         if(pfields[i].use) id = i;
      if(id +1 != nfields)
      {  int l, j, nf;
         nfields = id+1;
         l = strlen(FormatStr);
         nf = 0;
         for(i=0;i<l;i++)
         {  if(FormatStr[i] != '%') continue;
            nf++;
            if(nf == nfields)
            {  for(j=i+1; j<l; j++)
                 if(FormatStr[j] <= 32)
                 {  FormatStr[j] = 0;
                    break;
                 }
               break;
            }
         }

      }


      rc = 0;
      if(!NstrAnalyzed)    //Bad form file
      {  rc = 2;
      }
      return rc;
   }

   /* Read string from formfile(project), ignoger comments,
      and parse string ClassName.Name=par to ClassName Name par
    */
   int ReadStr(FILE *fp, char *str, char *nameClass, char *name, char *par )
   {  int i,iscomment,l;
      char *pstr;
/* читаем строку */
   M: pstr= fgets(str,128,fp);
      if(pstr == NULL) return 1; //EOF
/* игнорируем строки нулевой длины */
      l = strlen(str);
      if(str[l-1] == '\n')
                  str[--l] = 0;
      if(l == 0) goto M;
/* игнорируем комментарии */
      iscomment = 0;
      for(i=0;i<l;i++)
      { if(str[i] > 32)
         {   if(str[i] == ';') iscomment = 1;
             break;
         }
      }
      if(iscomment) goto M;
      pstr = strstr(str,"=");
      if(pstr == NULL)
         goto M; // игнорируем строки без "="
      *pstr = 0;
      strcpy(par,pstr+1);      // читаем параметры
      pstr = strstr(str,".");  // ищем точку для определения имени класса
      if(pstr == NULL)
          goto M; // игнорируем строки без "."
      *pstr = 0;
      sscanf(str,"%s",nameClass);
      sscanf(pstr+1,"%s",name); // читаем имя параметра
      return 0;
   }

/* запись запоминаемых параметров */
   int Write(FILE *fp)
   {  char *prefix="form.";
      struct tm *newtime;
      int i;
      time_t cur_t = time(NULL);

      /* Get the time in seconds */
       time(&cur_t);

      /* Convert it to the structure tm */
       newtime = localtime(&cur_t);

      fprintf(fp,";%s\n",AppName);
      fprintf(fp,";%s %s",FormName,asctime(newtime));
      fprintf(fp,";format string\n");
      fprintf(fp,"%sFormat=%s\n", prefix,FormatStr);
      fprintf(fp,"%sDimX=%i\n",  prefix,DimX);
      fprintf(fp,"%sDimY=%i\n",  prefix,DimY);
      fprintf(fp,"%sFileOut=%s\n",  prefix,FileOut);

      fprintf(fp,"%sBackColor=%x\n",  prefix,BackColor);
      fprintf(fp,"%sTitle=%s\n",  prefix,Title);

      fprintf(fp,"%sGridColor=%x\n",prefix,GridColor);
      fprintf(fp,"%sGridXon=%i\n",  prefix,GridXon);
      fprintf(fp,"%sGridYon=%i\n",  prefix,GridYon);
      fprintf(fp,"%sFormatX=%s\n",  prefix,FormatX);
      fprintf(fp,"%sFormatY=%s\n",  prefix,FormatY);


      fprintf(fp,";field properties\n");
      for(i=0;i<nfields;i++)
      {   pfields[i].Write(fp);
      }

       return 0;
   }

   int AnalizeRecodrRead(char *name, char *par)// анализ прочитанной строки конфига
   {
      int i,is=0,npar=0;
      int j,l;
      static char *psNames[]=
        { "Format","DimX","DimY","Title","BackColor",
          "GridColor","TitleColor", "TitleFont", "GridXon",   "GridYon",
          "FormatX",   "FormatY",   "FileOut",   "LegendUse", "LegendStartX",
          "LegendStartY","AxesColor","AxesNumColor","AxesNumFont","Locale",
          NULL
        };

     for(i=0;psNames[i];i++)
     {   if(!strcmp(name,psNames[i]))
         {  is = 1;
            npar = i;
            break;
         }
     }
     if(!is) return 1;
     switch(npar)
     {  case 0:
           strcpy(FormatStr,par);
/***************************************************/
        /* анализируем format string */
         l = strlen(FormatStr);
         for(i=0;i<l;i++)
         {  if(FormatStr[i] != '%') continue;
            for(j=i+1;j<l;j++)
            {
               if(FormatStr[j] == 'i' || FormatStr[j] == 'd' || FormatStr[j] == 'o' ||
                  FormatStr[j] == 'x' || FormatStr[j] == 'X')
                {  AddField(FIELDINT);  i = j;
                    break;
                }
               if(FormatStr[j] == 'u' )
                {  AddField(FIELDUINT); i = j;
                   break;
                }
               if(FormatStr[j] == 'e' || FormatStr[j] == 'f' || FormatStr[j] == 'g' ||
                  FormatStr[j] == 'E' || FormatStr[j] == 'G')
                {  if(FormatStr[j-1] == 'l')
                           AddField(FIELDDOUBLE);
                   else
                           AddField(FIELDFLOAT);
                   i = j;
                   break;
                }
               if(FormatStr[j] == 'c' )
                {  AddField(FIELDCHAR);   i = j;
                    break;
                }
               if(FormatStr[j] == 's' )
                {  AddField(FIELDSTRING); i = j;
                   break;
                }
               if(FormatStr[j] == 'D' )
                {  AddField(FIELDDATE); i = j;
                   FormatStr[j] = 's'; //Date read as string
                   break;
                }
               if(FormatStr[j] == 'H' ) //Hours
                {  AddField(FIELDTIME); i = j;
                   FormatStr[j] = 's'; //Time read as string
                   break;
                }
               if(FormatStr[j] == 'T' )
                {  char str[256];
                   AddField(FIELDDATETIME); i = j;
                   AddField(FIELDDATETIME1); i = j;
                   FormatStr[j] = 's'; //Time read as string
                   strncpy(str,FormatStr,j+1);
                   strcat(str," %s");
                   strcat(str,&FormatStr[j+2]);
                   l = strlen(str);
                   strcpy(FormatStr,str);
                   isDataTimeX = 2;
                   break;
                }
            } //endof for(j
         } //endof for(i

/* рассчитываем смещения */
        offsets[0] = 0;
        for(i=1;i<=nfields;i++)
        {   j = 4;
            if(pfields[i-1].type == FIELDCHAR) j = 1;
            else
                 if(pfields[i-1].type == FIELDDOUBLE) j = sizeof(double);
            offsets[i] = offsets[i-1] + j;
        }
        recordLen = offsets[nfields];

/***************************************************/
         break;
        case 1:
           sscanf(par,"%d", &DimX);
           break;
        case 2:
           sscanf(par,"%d", &DimY);
           break;
        case 3:
           strcpy(Title,par);
           break;
        case 4:
           sscanf(par,"%x", &BackColor);
           break;
        case 5:
           sscanf(par,"%x", &GridColor);
           break;
        case 6:
           sscanf(par,"%x", &TitleColor);
           break;
        case 7: //   TitleFont
          if(!stricmp(par,"T"))      TitleFont = 0;
          else if(!stricmp(par,"S")) TitleFont = 1;
          else if(!stricmp(par,"M")) TitleFont = 2;
          else if(!stricmp(par,"L")) TitleFont = 3;
          else if(!stricmp(par,"G")) TitleFont = 4;
           break;
        case 8:
           sscanf(par,"%x", &GridXon);
           break;
        case 9:
           sscanf(par,"%x", &GridYon);
           break;
        case 10:
           strcpy(FormatX, par);
           break;
        case 11:
           strcpy(FormatY, par);
           break;
        case 12:
           strcpy(FileOut, par);
           break;
        case 13:
           sscanf(par,"%d", &LegendUse);
           break;
        case 14:
           sscanf(par,"%d", &LegendStartX);
           break;
        case 15:
           sscanf(par,"%i", &LegendStartY);
           break;
        case 16:
           sscanf(par,"%x", &AxesColor);
           break;
        case 17:
           sscanf(par,"%x", &AxesNumColor);
           break;
        case 18:
          if(!stricmp(par,"T"))      AxesNumFont = 0;
          else if(!stricmp(par,"S")) AxesNumFont = 1;
          else if(!stricmp(par,"M")) AxesNumFont = 2;
          else if(!stricmp(par,"L")) AxesNumFont = 3;
          else if(!stricmp(par,"G")) AxesNumFont = 4;
           break;
        case 19:
           sscanf(par,"%x", &Locale);
           useLocale = 1;
           break;
     }
     return 0;
  }



};
