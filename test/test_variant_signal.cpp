#include <gmock/gmock.h>
#include <wkttool/variant_signal.h>

#include <string>

using ::testing::MockFunction;
using ::testing::StrictMock;
using Signal = wkttool::VariantSignal<wkttool::TypeList<int, std::string> >;
TEST(TestVariantSignal, CorrectListenerIsNotified) {
  Signal sig;
  StrictMock<MockFunction<void(const std::string &)> > mock_string_listener;
  StrictMock<MockFunction<void(const int &)> > mock_int_listener;
  sig.connect(mock_string_listener.AsStdFunction());
  sig.connect(mock_int_listener.AsStdFunction());

  EXPECT_CALL(mock_string_listener, Call("Hello"));
  EXPECT_CALL(mock_int_listener, Call(42));

  sig(std::string("Hello"));
  sig(42);
}
TEST(TestVariantSignal, CorrectListenerIsNotifiedImplicitConversion) {
  Signal sig;
  StrictMock<MockFunction<void(const std::string &)> > mock_string_listener;
  sig.connect(mock_string_listener.AsStdFunction());

  EXPECT_CALL(mock_string_listener, Call("Hello"));

  sig("Hello");
}
TEST(TestVariantSignal, LambdaListenerIsNotified) {
  Signal sig;
  bool called = false;
  sig.connect([&called](const std::string &) { called = true; });

  sig(std::string("Hello"));
  EXPECT_TRUE(called);
}
TEST(TestVariantSignal, FunctorListenerIsNotified) {
  Signal sig;
  StrictMock<MockFunction<void(void)> > fn;
  struct Functor {
    std::function<void(void)> fn;
    void operator()(const std::string &) { fn(); }
  };
  Functor f{fn.AsStdFunction()};
  sig.connect(f);

  EXPECT_CALL(fn, Call());
  sig(std::string("Hello"));
}
TEST(TestVariantSignal, NoNotificationAfterDisconnect) {
  Signal sig;
  StrictMock<MockFunction<void(const std::string &)> > mock_string_listener;
  auto connection = sig.connect(mock_string_listener.AsStdFunction());
  connection.disconnect();
  // If this triggers a notification, the strict mock will complain
  sig(std::string("Hello"));
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
