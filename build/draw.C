{

	TNtuple t("t","","PE");
	t.ReadFile("1MeV_gamma_4m");

	t.Draw("PE>>h(100,0,0)");
	h->Fit("gaus");
}
