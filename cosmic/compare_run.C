void compare_run(){
    TString dirIn = "/store/users/ovtin/cosmruns/results/cosm_stability/";
    //TString rootFile = "exp_cosm.root";
    TString rootFile = "exp_cosm_test.root";

    TString dirOut = dirIn + "compare_test5";

    gSystem->Exec("mkdir "+ dirOut);
    gSystem->Exec("cp /home/ovtin/public_html/index.php "+ dirOut);

    TString file = dirIn + rootFile;

    TFile *myFile = TFile::Open(file);

    TCanvas c("c","c",1400,800);
    c.SetGrid();
    c.cd();

    TProfile *sumpr1;
    TProfile *sumpr2;

    int i = 0;

    int firstCNT = 0;
    int lastCNT = 160;

    //std::list<int> CNTnumbers = {16, 18, 20, 21, 23, 24, 27, 29, 30, 36, 44, 49, 50, 56, 61, 62, 63, 64, 65, 67, 78};

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

	if(cnt==51) continue;
	if(cnt==59) continue;
        if(cnt==60) continue;
        if(cnt==80) continue;
        if(cnt==82) continue;
        if(cnt==155) continue;

	TProfile* pr1 = (TProfile*)myFile->Get(TString::Format("Run_Cnt%d",cnt).Data());
        pr1->Rebin(5);
	float avr1;
	int nbinsavr1 = 4;
	for(int k=1; k<=nbinsavr1; k++)
	{
	    avr1 += pr1->GetBinContent(k);
	    cout<<pr1->GetBinContent(k)<<endl;
	}
	avr1 = avr1/nbinsavr1;
	if(withNorm) pr1->Scale(1/avr1);
	pr1->GetListOfFunctions()->ls();
	TF1 *fun = pr1->GetFunction("myfit");
	pr1->GetListOfFunctions()->Remove(fun);
	pr1->SetLineColor(kRed);
	pr1->SetMarkerColor(kRed);
	pr1->SetMarkerStyle(4);
	pr1->SetName("Aerogel");
	if(withNorm) pr1->GetYaxis()->SetTitle("N_{ph.e.}/Nref_{ph.e.}");
	pr1->SetStats(0);
	pr1->GetXaxis()->SetNdivisions(205);
	pr1->GetYaxis()->SetNdivisions(510);
	pr1->Draw();

	TProfile* pr2 = (TProfile*)myFile->Get(TString::Format("Run_Cnt%d_shifter",cnt).Data());
        pr2->Rebin(5);
	float avr2;
	int nbinsavr2 = 4;
	for(int k=1; k<=nbinsavr2; k++)
	{
	    avr2 += pr2->GetBinContent(k);
	    cout<<pr2->GetBinContent(k)<<endl;
	}
	avr2 = avr2/nbinsavr2;
	if(withNorm) pr2->Scale(1/avr2);
	pr2->GetListOfFunctions()->ls();
	TF1 *fun2 = pr2->GetFunction("myfit2");
	pr2->GetListOfFunctions()->Remove(fun2);
	pr2->SetLineColor(kBlue);
	pr2->SetMarkerColor(kBlue);
	pr2->SetMarkerStyle(21);
	pr2->SetName("Shifter");
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

    //gStyle->SetOptStat(1000000001);

    //TF1 *myfit1 = new TF1("fitAer","[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1603714452);
    //TF1 *myfit1 = new TF1("fitAer","[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1680870067);
    //myfit1->SetParLimits(0,0,10000);
    //myfit1->SetParLimits(1,500000,1e12);
    //myfit1->SetParLimits(2,0,10000);
    //myfit1->SetParNames("p0","#tau","Const. level");
    //sumpr1->Fit("fitAer","","",1400778000,1603714452);
    //sumpr1->Fit("fitAer","","",1400778000,1680870067);
    //sumpr1->SetStats(1000000001);
    //gStyle->SetOptFit(kTRUE);
    /*
    if(withNorm)
    {
	sumpr1->SetMaximum(1.2);
    }
    else
    {
	sumpr1->SetMaximum(30.0);
    }
    */
    //sumpr1->GetXaxis()->SetNdivisions(205);
    sumpr1->Draw();

    //TPaveStats *s = (TPaveStats*) gPad->GetPrimitive("stats");
    //s->SetName("Aerogel");
    //s->SetX1NDC(0.5);
    //s->SetX2NDC(0.7);
    //s->SetY1NDC(0.8);
    //s->SetTextColor(kRed);

    //TF1 *myfit2 = new TF1("fitShifter","[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1603714452);
    //TF1 *myfit2 = new TF1("fitShifter","[0]*exp(-(x-1400778000)/[1])+[2]",1400778000,1680870067);
    //myfit2->SetParLimits(0,0,10000);
    //myfit2->SetParLimits(1,500000,1e12);
    //myfit2->SetParLimits(2,0,10000);
    //myfit2->SetParNames("p0","#tau","Const. level");
    //gStyle->SetOptFit(kTRUE);
    //sumpr2->SetStats(1000000001);
    //myfit2->SetLineColor(kBlue);
    //sumpr2->Fit("fitShifter","","sames",1400778000,1603714452);
    //sumpr2->Fit("fitShifter","","sames",1400778000,1680870067);
    //sumpr2->GetXaxis()->SetNdivisions(205);
    sumpr2->Draw("sames");
    gPad->Update();
    //TPaveStats *st2 = (TPaveStats*)sumpr2->FindObject("stats");
    //st2->SetName("Shifter");
    //st2->SetX1NDC(0.7);
    //st2->SetX2NDC(0.9);
    //st2->SetY1NDC(0.8);
    //st2->SetTextColor(kBlue);
    /*
    float resAerogel0 = myfit1->GetParameter(0)*exp(-(1400778000-1400778000)/myfit1->GetParameter(1))+myfit1->GetParameter(2);
    float resShifter0 = myfit2->GetParameter(0)*exp(-(1400778000-1400778000)/myfit2->GetParameter(1))+myfit2->GetParameter(2);
    //float resAerogel = myfit1->GetParameter(0)*exp(-(1603714452-1400778000)/myfit1->GetParameter(1))+myfit1->GetParameter(2);
    //float resShifter = myfit2->GetParameter(0)*exp(-(1603714452-1400778000)/myfit2->GetParameter(1))+myfit2->GetParameter(2);
    float resAerogel = myfit1->GetParameter(0)*exp(-(1680870067-1400778000)/myfit1->GetParameter(1))+myfit1->GetParameter(2);
    float resShifter = myfit2->GetParameter(0)*exp(-(1680870067-1400778000)/myfit2->GetParameter(1))+myfit2->GetParameter(2);

    cout<<"resAerogel0="<<resAerogel0<<"\t"<<"resShifter0="<<resShifter0<<endl;
    cout<<"resAerogel="<<resAerogel<<"\t"<<"resShifter="<<resShifter<<endl;

    cout<<"Aerogel + QE degradation:"<<(resAerogel0-resAerogel)*100<<endl;
    cout<<"QE degradation:"<<(resShifter0-resShifter)*100<<endl;
    cout<<"Aerogel degradation:"<<(resAerogel0-resAerogel)*100-(resShifter0-resShifter)*100<<endl;
    */
    if(withNorm && fromlist1==false && fromlist2==false){
	c1.Print(dirOut + "/" + TString::Format("cnt_sum_norm_run%d-%d.png",firstCNT+1,lastCNT).Data());
	c1.Print(dirOut + "/" + TString::Format("cnt_sum_norm_run%d-%d.root",firstCNT+1,lastCNT).Data());
    }
    else if(withNorm && fromlist1){
	c1.Print(dirOut + "/" + "cnt_sum_norm_fromlist1_run.png");
	c1.Print(dirOut + "/" + "cnt_sum_norm_fromlist1_run.root");
    }
    else if(withNorm && fromlist2){
	c1.Print(dirOut + "/" + "cnt_sum_norm_fromlist2_run.png");
	c1.Print(dirOut + "/" + "cnt_sum_norm_fromlist2_run.root");
    }
    else if(fromlist1){
	c1.Print(dirOut + "/" + "cnt_sum_fromlist1_run.png");
	c1.Print(dirOut + "/" + "cnt_sum_fromlist1_run.root");
    }
    else if(fromlist2){
	c1.Print(dirOut + "/" + "cnt_sum_fromlist2_run.png");
	c1.Print(dirOut + "/" + "cnt_sum_fromlist2_run.root");
    }
    else{
	c1.Print(dirOut + "/" + TString::Format("cnt_sum_run%d-%d.png",firstCNT+1,lastCNT).Data());
	c1.Print(dirOut + "/" + TString::Format("cnt_sum_run%d-%d.root",firstCNT+1,lastCNT).Data());
    }
}
