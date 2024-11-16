void compare(){
    TString dirIn = "/store/users/ovtin/cosmruns/results/cosm_stability/";
    TString rootFile = "exp_cosm.root";

    TString dirOut = dirIn + "compare";

    gSystem->Exec("mkdir "+ dirOut);
    gSystem->Exec("cp /home/ovtin/public_html/index.php "+ dirOut);

    TString file = dirIn + rootFile;

    TFile *myFile = TFile::Open(file);

    TCanvas c("c","c",1400,800);
    c.SetGrid();
    c.cd();

    TProfile *sumpr1;
    TProfile *sumpr2;
    TProfile *sumpr3;

    int i = 0;

    //int firstCNT = 0;
    int firstCNT = 80;
    int lastCNT = 160;
    //int lastCNT = 80;

    bool doRebin = 1;

    bool withNorm = 0;
    bool fromlist1 = 0; //16-78
    bool fromlist2 = 0; //all-(16-78)

    for(int cnt=firstCNT; cnt<lastCNT; cnt++)
    {
        gStyle->SetOptStat(0);

        int numbers[] = {15, 17, 19, 20, 22, 23, 26, 28, 29, 35, 43, 48, 49, 55, 60, 61, 62, 63, 64, 66, 77};
	int *numbers_end = numbers + sizeof(numbers)/sizeof(numbers[0]);
	int skip=0;
        if(fromlist1) skip=1;
	for (int *it = numbers; it != numbers_end; ++it)
	{
	    if( fromlist1 && cnt==*it ) skip=0;
	    if( fromlist2 && cnt==*it ) skip=1;
	}

        if(skip==1) continue;
	cout<<"Counter="<<cnt<<endl;

	///////////////////////////
	if(cnt==46) continue;
	if(cnt==51) continue;
	if(cnt==59) continue;
        if(cnt==60) continue;
        if(cnt==66) continue;
        if(cnt==68) continue;
        if(cnt==80) continue;
        if(cnt==82) continue;
        if(cnt==91) continue;
        if(cnt==96) continue;
        if(cnt==143) continue;
        if(cnt==155) continue;
        //////////////////////////


	TProfile* pr1 = (TProfile*)myFile->Get(TString::Format("Cnt%d",cnt).Data());
        if(doRebin) pr1->Rebin(5);
	float avr1 = 0.;
	int nbinsavr1 = 4;
        int norm1 = 0.;
	for(int k=1; k<=nbinsavr1; k++)
	{
	    if( pr1->GetBinContent(k)>0. )
	    {
		avr1 += pr1->GetBinContent(k);
                norm1++;
	    }
	    cout<<pr1->GetBinContent(k)<<endl;
	}
	avr1 = avr1/norm1;
	if(withNorm) pr1->Scale(1/avr1);
	pr1->GetListOfFunctions()->ls();
	TF1 *fun = pr1->GetFunction("myfit");
	pr1->GetListOfFunctions()->Remove(fun);
	pr1->SetLineColor(kRed);
	pr1->SetMarkerColor(kRed);
	pr1->SetMarkerStyle(4);
	pr1->SetName("Total");
	if(withNorm) pr1->GetYaxis()->SetTitle("N_{ph.e.}/Nref_{ph.e.}");
	pr1->SetStats(0);
	pr1->GetXaxis()->SetNdivisions(205);
	pr1->GetYaxis()->SetNdivisions(510);
	pr1->Draw();

	TProfile* pr2 = (TProfile*)myFile->Get(TString::Format("Cnt%d_shifter",cnt).Data());
        if(doRebin) pr2->Rebin(5);
	float avr2 = 0.;
	int nbinsavr2 = 4;
        int norm2 = 0.;
	for(int k=1; k<=nbinsavr2; k++)
	{
	    if( pr2->GetBinContent(k)>0. )
	    {
		avr2 += pr2->GetBinContent(k);
                norm2++;
	    }
	    cout<<pr2->GetBinContent(k)<<endl;
	}
	avr2 = avr2/norm2;
	if(withNorm) pr2->Scale(1/avr2);
	pr2->GetListOfFunctions()->ls();
	TF1 *fun2 = pr2->GetFunction("myfit2");
	pr2->GetListOfFunctions()->Remove(fun2);
	pr2->SetLineColor(kBlue);
	pr2->SetMarkerColor(kBlue);
	pr2->SetMarkerStyle(21);
	pr2->SetName("WLS");
        if(withNorm) pr2->GetYaxis()->SetTitle("N_{ph.e.}/Nref_{ph.e.}");
        pr2->SetStats(0);
	pr2->GetXaxis()->SetNdivisions(205);
	pr2->GetYaxis()->SetNdivisions(510);
	pr2->Draw("same");

	c.Update();
	c.Print(dirOut + "/" + TString::Format("cnt%d.png",cnt).Data());

	i++;
	if(i==1)
	{
	    sumpr1 = new TProfile(*pr1);
	    sumpr2 = new TProfile(*pr2);
	}
	else{
	    sumpr1->Add(sumpr1,pr1);
	    sumpr2->Add(sumpr2,pr2);
	}
    }

    TCanvas c1("c1","c1",1400,800);
    c1.SetGrid();
    c1.cd();

    gStyle->SetOptStat(1000000001);

    //TF1 *myfit1 = new TF1("fitAer","[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1603714452);
    TF1 *myfit1 = new TF1("fitAer","[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1680870067);  //2023
    //TF1 *myfit1 = new TF1("fitAer","[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1654843524);  //2022
    myfit1->SetParLimits(0,0,10000);
    myfit1->SetParLimits(1,500000,1e12);
    myfit1->SetParLimits(2,0,10000);
    myfit1->SetParNames("p0","#tau","Const. level");
    //sumpr1->Fit("fitAer","","",1400778000,1603714452);
    sumpr1->Fit("fitAer","","",1400778000,1680870067);  //2023
    //sumpr1->Fit("fitAer","","",1400778000,1654843524);  //2022
    sumpr1->SetStats(1000000001);
    gStyle->SetOptFit(kTRUE);
    if(withNorm)
    {
	sumpr1->SetMaximum(1.4);
    }
    else
    {
	sumpr1->SetMaximum(25.0);
    }
    //sumpr1->GetXaxis()->SetNdivisions(205);
    //sumpr1->GetXaxis()->SetRangeUser(1400778000,1654843524);  //2022
    sumpr1->Draw();

    TPaveStats *s = (TPaveStats*) gPad->GetPrimitive("stats");
    s->SetName("Total");
    s->SetX1NDC(0.20);
    s->SetX2NDC(0.44);
    s->SetY1NDC(0.76);
    s->SetTextColor(kRed);

    //TF1 *myfit2 = new TF1("fitShifter","[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1603714452);
    TF1 *myfit2 = new TF1("fitShifter","[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1680870067); //2023
    //TF1 *myfit2 = new TF1("fitShifter","[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1654843524);   //2022
    myfit2->SetParLimits(0,0,10000);
    myfit2->SetParLimits(1,500000,1e12);
    myfit2->SetParLimits(2,0,10000);
    myfit2->SetParNames("p0","#tau","Const. level");
    gStyle->SetOptFit(kTRUE);
    sumpr2->SetStats(1000000001);
    myfit2->SetLineColor(kBlue);
    //sumpr2->Fit("fitShifter","","sames",1400778000,1603714452);
    sumpr2->Fit("fitShifter","","sames",1400778000,1680870067); //2023
    //sumpr2->Fit("fitShifter","","sames",1400778000,1654843524);
    //sumpr2->GetXaxis()->SetNdivisions(205);
    //sumpr2->GetXaxis()->SetRangeUser(1400778000,1654843524);  //2022
    sumpr2->Draw("sames");
    gPad->Update();
    TPaveStats *st2 = (TPaveStats*)sumpr2->FindObject("stats");
    st2->SetName("WLS");
    st2->SetX1NDC(0.44);
    st2->SetX2NDC(0.68);
    st2->SetY1NDC(0.76);
    st2->SetTextColor(kBlue);

    float resTotal0 = myfit1->GetParameter(0)*exp(-(1400778000-1400778000)/myfit1->GetParameter(1))+myfit1->GetParameter(2);
    float resShifter0 = myfit2->GetParameter(0)*exp(-(1400778000-1400778000)/myfit2->GetParameter(1))+myfit2->GetParameter(2);
    //float resAerogel = myfit1->GetParameter(0)*exp(-(1603714452-1400778000)/myfit1->GetParameter(1))+myfit1->GetParameter(2);
    //float resShifter = myfit2->GetParameter(0)*exp(-(1603714452-1400778000)/myfit2->GetParameter(1))+myfit2->GetParameter(2);
    //2022
    //float resAerogel = myfit1->GetParameter(0)*exp(-(1654843524-1400778000)/myfit1->GetParameter(1))+myfit1->GetParameter(2);
    //float resShifter = myfit2->GetParameter(0)*exp(-(1654843524-1400778000)/myfit2->GetParameter(1))+myfit2->GetParameter(2);
    //2023
    float resTotal = myfit1->GetParameter(0)*exp(-(1680870067-1400778000)/myfit1->GetParameter(1))+myfit1->GetParameter(2);
    float resShifter = myfit2->GetParameter(0)*exp(-(1680870067-1400778000)/myfit2->GetParameter(1))+myfit2->GetParameter(2);

    cout<<"resTotal0="<<resTotal0<<"\t"<<"resShifter0="<<resShifter0<<endl;
    cout<<"resTotal="<<resTotal<<"\t"<<"resShifter="<<resShifter<<endl;

    cout<<"Aerogel + QE degradation1:"<<(resTotal0-resTotal)*100<<endl;
    cout<<"Aerogel degradation1:"<<(resTotal0-resTotal)*100-(resShifter0-resShifter)*100<<endl;
    cout<<"QE degradation:"<<(resShifter0-resShifter)*100<<endl;

    if(withNorm)
    {
	TH1D *h1 = sumpr1->ProjectionX();
	TH1D *h2 = sumpr2->ProjectionX();
	Double_t coef1 = resShifter0/resTotal0;
	//h1->Multiply(h2);
	h1->Divide(h2);
	h1->Scale(coef1);
	h1->SetLineColor(28);
	h1->SetMarkerColor(28);
	h1->SetMarkerStyle(21);
	h1->SetName("Aerogel");
	//h1->Draw("same");

	TF1 *myfit3 = new TF1("fitAer0","[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1680870067); //2023
	myfit3->SetParLimits(0,0,10000);
	myfit3->SetParLimits(1,500000,1e12);
	myfit3->SetParLimits(2,0,10000);
	myfit3->SetParNames("p0","#tau","Const. level");
	gStyle->SetOptFit(kTRUE);
	h1->SetStats(1000000001);
	myfit3->SetLineColor(28);
	h1->Fit("fitAer0","","sames",1400778000,1680870067); //2023
	h1->Draw("sames");
	gPad->Update();
	TPaveStats *st3 = (TPaveStats*)h1->FindObject("stats");
	st3->SetName("Aerogel");
	st3->SetX1NDC(0.68);
	st3->SetX2NDC(0.92);
	st3->SetY1NDC(0.76);
	st3->SetTextColor(28);

	float resAerogel0 = myfit3->GetParameter(0)*exp(-(1400778000-1400778000)/myfit3->GetParameter(1))+myfit3->GetParameter(2);
	float resAerogel = myfit3->GetParameter(0)*exp(-(1680870067-1400778000)/myfit3->GetParameter(1))+myfit3->GetParameter(2);
	cout<<"Aerogel degradation2:"<<(resAerogel0-resAerogel)*100<<endl;
    }

    if(withNorm && fromlist1==false && fromlist2==false){
	c1.Print(dirOut + "/" + TString::Format("cnt_sum_norm%d-%d.png",firstCNT+1,lastCNT).Data());
	c1.Print(dirOut + "/" + TString::Format("cnt_sum_norm%d-%d.root",firstCNT+1,lastCNT).Data());
    }
    else if(withNorm && fromlist1){
	c1.Print(dirOut + "/" + "cnt_sum_norm_fromlist1.png");
	c1.Print(dirOut + "/" + "cnt_sum_norm_fromlist1.root");
    }
    else if(withNorm && fromlist2){
	//c1.Print(dirOut + "/" + "cnt_sum_norm_fromlist2.png");
	//c1.Print(dirOut + "/" + "cnt_sum_norm_fromlist2.root");
	c1.Print(dirOut + "/" + "cnt_sum_norm_fromlist2_80-21.png");
	c1.Print(dirOut + "/" + "cnt_sum_norm_fromlist2_80-21.root");
    }
    else if(fromlist1){
	c1.Print(dirOut + "/" + "cnt_sum_fromlist1.png");
	c1.Print(dirOut + "/" + "cnt_sum_fromlist1.root");
    }
    else if(fromlist2){
	//c1.Print(dirOut + "/" + "cnt_sum_fromlist2.png");
	//c1.Print(dirOut + "/" + "cnt_sum_fromlist2.root");
	c1.Print(dirOut + "/" + "cnt_sum_fromlist2_80-21.png");
	c1.Print(dirOut + "/" + "cnt_sum_fromlist2_80-21.root");
    }
    else{
	c1.Print(dirOut + "/" + TString::Format("cnt_sum%d-%d.png",firstCNT+1,lastCNT).Data());
	c1.Print(dirOut + "/" + TString::Format("cnt_sum%d-%d.root",firstCNT+1,lastCNT).Data());
    }
}
